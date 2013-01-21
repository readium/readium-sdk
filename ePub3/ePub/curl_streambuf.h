//
//  curl_streambuf.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-17.
//  Copyright (c) 2012-2013 The Readium Foundation.
//  
//  The Readium SDK is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef ePub3_curl_streambuf_h
#define ePub3_curl_streambuf_h

#include "epub3.h"
#include <curl/curl.h>

EPUB3_BEGIN_NAMESPACE

// modelled on std::filebuf. I'll make it a ring buffer too, at some point
template <class _CharT, class _Traits = std::char_traits<_CharT> >
class basic_curlbuf : public std::basic_streambuf<_CharT, _Traits>
{
public:
    typedef _CharT                           char_type;
    typedef _Traits                          traits_type;
    typedef typename traits_type::int_type   int_type;
    typedef typename traits_type::pos_type   pos_type;
    typedef typename traits_type::off_type   off_type;
    typedef typename traits_type::state_type state_type;
    
public:
    // 27.9.1.2 Constructors/destructor:
    basic_curlbuf();
    basic_curlbuf(basic_curlbuf&& __rhs);
    virtual ~basic_curlbuf();
    
    // 27.9.1.3 Assign/swap:
    basic_curlbuf& operator=(basic_curlbuf&& __rhs);
    void swap(basic_curlbuf& __rhs);
    
    // 27.9.1.4 Members:
    bool is_open() const;
    basic_curlbuf* open(const char* __s, std::ios_base::openmode __mode);
    basic_curlbuf* open(const std::string& __s, std::ios_base::openmode __mode);
    basic_curlbuf* close();
    
    // custom
    template <typename... Args>
    bool SetOption(CURLoption opt, const Args&... args);
    
    static bool CanHandleURLScheme(const std::string& url);
    
protected:
    // 27.9.1.5 Overridden virtual functions:
    virtual int_type underflow();
    virtual int_type pbackfail(int_type __c = traits_type::eof());
    virtual int_type overflow (int_type __c = traits_type::eof());
    virtual std::basic_streambuf<char_type, traits_type>* setbuf(char_type* __s, std::streamsize __n);
    virtual pos_type seekoff(off_type __off, std::ios_base::seekdir __way,
                             std::ios_base::openmode __wch = std::ios_base::in | std::ios_base::out);
    virtual pos_type seekpos(pos_type __sp,
                             std::ios_base::openmode __wch = std::ios_base::in | std::ios_base::out);
    virtual int sync();
    virtual void imbue(const std::locale& __loc);
    
private:
    char*       __extbuf_;
    const char* __extbufnext_;
    const char* __extbufend_;
    char __extbuf_min_[8];
    size_t __ebs_;
    char_type* __intbuf_;
    size_t __ibs_;
    void* __curl_;
    const std::codecvt<char_type, char, state_type>* __cv_;
    state_type __st_;
    std::ios_base::openmode __om_;
    std::ios_base::openmode __cm_;
    bool __owns_eb_;
    bool __owns_ib_;
    bool __always_noconv_;
    
    bool __read_mode();
    void __write_mode();
    
    static const std::map<std::string, bool>  __curl_schemes_;
    bool curl_init(const std::string& url);
    size_t curl_read(void *__buf, size_t __nmemb);
    size_t curl_write(void *__buf, size_t __nmemb);
    
};

template <class _CharT, class _Traits>
basic_curlbuf<_CharT, _Traits>::basic_curlbuf()
    : __extbuf_(0),
      __extbufnext_(0),
      __extbufend_(0),
      __ebs_(0),
      __intbuf_(0),
      __ibs_(0),
      __curl_(nullptr),
      __cv_(&std::use_facet<std::codecvt<char_type, char, state_type> >(this->getloc())),
      __st_(),
      __om_(0),
      __cm_(0),
      __owns_eb_(false),
      __owns_ib_(false),
      __always_noconv_(__cv_->always_noconv())
{
    setbuf(0, 4096);
}

template <class _CharT, class _Traits>
basic_curlbuf<_CharT, _Traits>::basic_curlbuf(basic_curlbuf&& __rhs)
    : std::basic_streambuf<_CharT, _Traits>(__rhs)
{
    if (__rhs.__extbuf_ == __rhs.__extbuf_min_)
    {
        __extbuf_ = __extbuf_min_;
        __extbufnext_ = __extbuf_ + (__rhs.__extbufnext_ - __rhs.__extbuf_);
        __extbufend_ = __extbuf_ + (__rhs.__extbufend_ - __rhs.__extbuf_);
    }
    else
    {
        __extbuf_ = __rhs.__extbuf_;
        __extbufnext_ = __rhs.__extbufnext_;
        __extbufend_ = __rhs.__extbufend_;
    }
    __ebs_ = __rhs.__ebs_;
    __intbuf_ = __rhs.__intbuf_;
    __ibs_ = __rhs.__ibs_;
    __curl_ = __rhs.__curl_;
    __cv_ = __rhs.__cv_;
    __st_ = __rhs.__st_;
    __om_ = __rhs.__om_;
    __cm_ = __rhs.__cm_;
    __owns_eb_ = __rhs.__owns_eb_;
    __owns_ib_ = __rhs.__owns_ib_;
    __always_noconv_ = __rhs.__always_noconv_;
    if (__rhs.pbase())
    {
        if (__rhs.pbase() == __rhs.__intbuf_)
            this->setp(__intbuf_, __intbuf_ + (__rhs. epptr() - __rhs.pbase()));
        else
            this->setp((char_type*)__extbuf_,
                       (char_type*)__extbuf_ + (__rhs. epptr() - __rhs.pbase()));
        this->pbump(__rhs. pptr() - __rhs.pbase());
    }
    else if (__rhs.eback())
    {
        if (__rhs.eback() == __rhs.__intbuf_)
            this->setg(__intbuf_, __intbuf_ + (__rhs.gptr() - __rhs.eback()),
                       __intbuf_ + (__rhs.egptr() - __rhs.eback()));
        else
            this->setg((char_type*)__extbuf_,
                       (char_type*)__extbuf_ + (__rhs.gptr() - __rhs.eback()),
                       (char_type*)__extbuf_ + (__rhs.egptr() - __rhs.eback()));
    }
    __rhs.__extbuf_ = 0;
    __rhs.__extbufnext_ = 0;
    __rhs.__extbufend_ = 0;
    __rhs.__ebs_ = 0;
    __rhs.__intbuf_ = 0;
    __rhs.__ibs_ = 0;
    __rhs.__curl_ = nullptr;
    __rhs.__st_ = state_type();
    __rhs.__om_ = 0;
    __rhs.__cm_ = 0;
    __rhs.__owns_eb_ = false;
    __rhs.__owns_ib_ = false;
    __rhs.setg(0, 0, 0);
    __rhs.setp(0, 0);
}

template <class _CharT, class _Traits>
inline _LIBCPP_INLINE_VISIBILITY
basic_curlbuf<_CharT, _Traits>&
basic_curlbuf<_CharT, _Traits>::operator=(basic_curlbuf&& __rhs)
{
    close();
    swap(__rhs);
}

template <class _CharT, class _Traits>
basic_curlbuf<_CharT, _Traits>::~basic_curlbuf()
{
    try
    {
        close();
    }
    catch (...)
    {
    }
    
    if ( __owns_eb_ )
        delete [] __extbuf_;
    if ( __owns_ib_ )
        delete [] __intbuf_;
}

template <class _CharT, class _Traits>
void
basic_curlbuf<_CharT, _Traits>::swap(basic_curlbuf& __rhs)
{
    std::basic_streambuf<char_type, traits_type>::swap(__rhs);
    if (__extbuf_ != __extbuf_min_ && __rhs.__extbuf_ != __rhs.__extbuf_min_)
    {
        _VSTD::swap(__extbuf_, __rhs.__extbuf_);
        _VSTD::swap(__extbufnext_, __rhs.__extbufnext_);
        _VSTD::swap(__extbufend_, __rhs.__extbufend_);
    }
    else
    {
        ptrdiff_t __ln = __extbufnext_ - __extbuf_;
        ptrdiff_t __le = __extbufend_ - __extbuf_;
        ptrdiff_t __rn = __rhs.__extbufnext_ - __rhs.__extbuf_;
        ptrdiff_t __re = __rhs.__extbufend_ - __rhs.__extbuf_;
        if (__extbuf_ == __extbuf_min_ && __rhs.__extbuf_ != __rhs.__extbuf_min_)
        {
            __extbuf_ = __rhs.__extbuf_;
            __rhs.__extbuf_ = __rhs.__extbuf_min_;
        }
        else if (__extbuf_ != __extbuf_min_ && __rhs.__extbuf_ == __rhs.__extbuf_min_)
        {
            __rhs.__extbuf_ = __extbuf_;
            __extbuf_ = __extbuf_min_;
        }
        __extbufnext_ = __extbuf_ + __rn;
        __extbufend_ = __extbuf_ + __re;
        __rhs.__extbufnext_ = __rhs.__extbuf_ + __ln;
        __rhs.__extbufend_ = __rhs.__extbuf_ + __le;
    }
    _VSTD::swap(__ebs_, __rhs.__ebs_);
    _VSTD::swap(__intbuf_, __rhs.__intbuf_);
    _VSTD::swap(__ibs_, __rhs.__ibs_);
    _VSTD::swap(__curl_, __rhs.__curl_);
    _VSTD::swap(__cv_, __rhs.__cv_);
    _VSTD::swap(__st_, __rhs.__st_);
    _VSTD::swap(__om_, __rhs.__om_);
    _VSTD::swap(__cm_, __rhs.__cm_);
    _VSTD::swap(__owns_eb_, __rhs.__owns_eb_);
    _VSTD::swap(__owns_ib_, __rhs.__owns_ib_);
    _VSTD::swap(__always_noconv_, __rhs.__always_noconv_);
    if (this->eback() == (char_type*)__rhs.__extbuf_min_)
    {
        ptrdiff_t __n = this->gptr() - this->eback();
        ptrdiff_t __e = this->egptr() - this->eback();
        this->setg((char_type*)__extbuf_min_,
                   (char_type*)__extbuf_min_ + __n,
                   (char_type*)__extbuf_min_ + __e);
    }
    else if (this->pbase() == (char_type*)__rhs.__extbuf_min_)
    {
        ptrdiff_t __n = this->pptr() - this->pbase();
        ptrdiff_t __e = this->epptr() - this->pbase();
        this->setp((char_type*)__extbuf_min_,
                   (char_type*)__extbuf_min_ + __e);
        this->pbump(__n);
    }
    if (__rhs.eback() == (char_type*)__extbuf_min_)
    {
        ptrdiff_t __n = __rhs.gptr() - __rhs.eback();
        ptrdiff_t __e = __rhs.egptr() - __rhs.eback();
        __rhs.setg((char_type*)__rhs.__extbuf_min_,
                   (char_type*)__rhs.__extbuf_min_ + __n,
                   (char_type*)__rhs.__extbuf_min_ + __e);
    }
    else if (__rhs.pbase() == (char_type*)__extbuf_min_)
    {
        ptrdiff_t __n = __rhs.pptr() - __rhs.pbase();
        ptrdiff_t __e = __rhs.epptr() - __rhs.pbase();
        __rhs.setp((char_type*)__rhs.__extbuf_min_,
                   (char_type*)__rhs.__extbuf_min_ + __e);
        __rhs.pbump(__n);
    }
}

template <class _CharT, class _Traits>
inline void swap(basic_curlbuf<_CharT, _Traits>& __x, basic_curlbuf<_CharT, _Traits>& __y)
{
    __x.swap(__y);
}

template <class _CharT, class _Traits>
bool basic_curlbuf<_CharT, _Traits>::is_open() const
{
    return __curl_ != nullptr;
}

template <class _CharT, class _Traits>
basic_curlbuf<_CharT, _Traits>*
basic_curlbuf<_CharT, _Traits>::open(const char *__s, std::ios_base::openmode __mode)
{
    basic_curlbuf<_CharT, _Traits>* __rt = nullptr;
    if ( __curl_ == nullptr )
    {
        __rt = this;
        __curl_ = curl_easy_init();
        if ( __curl_ != nullptr )
        {
            SetOption(CURLOPT_URL, __s);
            SetOption(CURLOPT_CONNECT_ONLY, 1);
            CURLcode __e = curl_easy_perform(__curl_);
            if ( __e == CURLE_OK )
                __om_ = __mode;     // for STL compatibility, even though we ignore it
            else
            {
                curl_easy_cleanup(__curl_);
                __rt = nullptr;
            }
        }
        else
        {
            __rt = nullptr;
        }
    }
    return __rt;
}

template <class _CharT, class _Traits>
basic_curlbuf<_CharT, _Traits>*
basic_curlbuf<_CharT, _Traits>::open(const std::string &__s, std::ios_base::openmode __mode)
{
    return open(__s.c_str(), __mode);
}

template <class _CharT, class _Traits>
basic_curlbuf<_CharT, _Traits>*
basic_curlbuf<_CharT, _Traits>::close()
{
    basic_curlbuf<_CharT, _Traits>* __rt = nullptr;
    if ( __curl_ != nullptr )
    {
        __rt = this;
        std::unique_ptr<CURL, void(*)(CURL*)> __h(__curl_, curl_easy_cleanup);
        if ( sync() )
            __rt = nullptr;
        curl_easy_cleanup(__h.release());
        __curl_ = nullptr;
    }
    return __rt;
}

template <class _CharT, class _Traits>
typename basic_curlbuf<_CharT, _Traits>::int_type
basic_curlbuf<_CharT, _Traits>::underflow()
{
    if (__curl_ == 0)
        return traits_type::eof();
    bool __initial = __read_mode();
    char_type __1buf;
    if (this->gptr() == 0)
        this->setg(&__1buf, &__1buf+1, &__1buf+1);
    const size_t __unget_sz = __initial ? 0 : std::min<size_t>((this->egptr() - this->eback()) / 2, 4);
    int_type __c = traits_type::eof();
    if (this->gptr() == this->egptr())
    {
        memmove(this->eback(), this->egptr() - __unget_sz, __unget_sz * sizeof(char_type));
        if (__always_noconv_)
        {
            size_t __nmemb = static_cast<size_t>(this->egptr() - this->eback() - __unget_sz);
            CURLcode __e = curl_easy_recv(__curl_, this->eback() + __unget_sz, __nmemb*sizeof(char_type), &__nmemb);
            if (__e == CURLE_OK && __nmemb != 0)
            {
                this->setg(this->eback(),
                           this->eback() + __unget_sz,
                           this->eback() + __unget_sz + __nmemb);
                __c = traits_type::to_int_type(*this->gptr());
            }
        }
        else
        {
            memmove(__extbuf_, __extbufnext_, __extbufend_ - __extbufnext_);
            __extbufnext_ = __extbuf_ + (__extbufend_ - __extbufnext_);
            __extbufend_ = __extbuf_ + (__extbuf_ == __extbuf_min_ ? sizeof(__extbuf_min_) : __ebs_);
            size_t __nmemb = _VSTD::min(static_cast<size_t>(this->egptr() - this->eback() - __unget_sz),
                                        static_cast<size_t>(__extbufend_ - __extbufnext_));
            std::codecvt_base::result __r;
            //state_type __svs = __st_;
            size_t __nr = 0;
            CURLcode __e = curl_easy_recv(__curl_, this->eback() + __unget_sz, __nmemb*sizeof(char_type), &__nr);
            if (__e == CURLE_OK && __nr != 0)
            {
                __extbufend_ = __extbufnext_ + __nr;
                char_type*  __inext;
                __r = __cv_->in(__st_, __extbuf_, __extbufend_, __extbufnext_,
                                this->eback() + __unget_sz,
                                this->egptr(), __inext);
                if (__r == std::codecvt_base::noconv)
                {
                    this->setg((char_type*)__extbuf_, (char_type*)__extbuf_, (char_type*)__extbufend_);
                    __c = traits_type::to_int_type(*this->gptr());
                }
                else if (__inext != this->eback() + __unget_sz)
                {
                    this->setg(this->eback(), this->eback() + __unget_sz, __inext);
                    __c = traits_type::to_int_type(*this->gptr());
                }
            }
        }
    }
    else
        __c = traits_type::to_int_type(*this->gptr());
    if (this->eback() == &__1buf)
        this->setg(0, 0, 0);
    return __c;
}

template <class _CharT, class _Traits>
typename basic_curlbuf<_CharT, _Traits>::int_type
basic_curlbuf<_CharT, _Traits>::pbackfail(int_type __c)
{
    if (__curl_ && this->eback() < this->gptr())
    {
        if (traits_type::eq_int_type(__c, traits_type::eof()))
        {
            this->gbump(-1);
            return traits_type::not_eof(__c);
        }
        if ((__om_ & std::ios_base::out) ||
            traits_type::eq(traits_type::to_char_type(__c), this->gptr()[-1]))
        {
            this->gbump(-1);
            *this->gptr() = traits_type::to_char_type(__c);
            return __c;
        }
    }
    return traits_type::eof();
}

template <class _CharT, class _Traits>
typename basic_curlbuf<_CharT, _Traits>::int_type
basic_curlbuf<_CharT, _Traits>::overflow(int_type __c)
{
    if (__curl_ == 0)
        return traits_type::eof();
    __write_mode();
    char_type __1buf;
    char_type* __pb_save = this->pbase();
    char_type* __epb_save = this->epptr();
    if (!traits_type::eq_int_type(__c, traits_type::eof()))
    {
        if (this->pptr() == 0)
            this->setp(&__1buf, &__1buf+1);
        *this->pptr() = traits_type::to_char_type(__c);
        this->pbump(1);
    }
    if (this->pptr() != this->pbase())
    {
        if (__always_noconv_)
        {
            size_t __nmemb = static_cast<size_t>(this->pptr() - this->pbase());
            size_t __nsent = 0;
            CURLcode __e = curl_easy_send(__curl_, this->pbase(), __nmemb*sizeof(char_type), &__nsent);
            if (__e != CURLE_OK || __nsent != __nmemb*sizeof(char_type))
                return traits_type::eof();
        }
        else
        {
            char* __extbe = __extbuf_;
            std::codecvt_base::result __r;
            do
            {
                const char_type* __e;
                __r = __cv_->out(__st_, this->pbase(), this->pptr(), __e,
                                 __extbuf_, __extbuf_ + __ebs_, __extbe);
                if (__e == this->pbase())
                    return traits_type::eof();
                if (__r == std::codecvt_base::noconv)
                {
                    size_t __nmemb = static_cast<size_t>(this->pptr() - this->pbase());
                    size_t __nsent = 0;
                    CURLcode __e = curl_easy_send(__curl_, this->pbase(), __nmemb*sizeof(char_type), &__nsent);
                    if (__e != CURLE_OK || __nsent != __nmemb*sizeof(char_type))
                        return traits_type::eof();
                }
                else if (__r == std::codecvt_base::ok || __r == std::codecvt_base::partial)
                {
                    size_t __nmemb = static_cast<size_t>(__extbe - __extbuf_);
                    size_t __nsent = 0;
                    CURLcode __e = curl_easy_send(__curl_, this->pbase(), __nmemb*sizeof(char_type), &__nsent);
                    if (__e != CURLE_OK || __nsent != __nmemb*sizeof(char_type))
                        return traits_type::eof();
                    if (__r == std::codecvt_base::partial)
                    {
                        this->setp((char_type*)__e, this->pptr());
                        this->pbump(int(this->epptr() - this->pbase()));
                    }
                }
                else
                    return traits_type::eof();
            } while (__r == std::codecvt_base::partial);
        }
        this->setp(__pb_save, __epb_save);
    }
    return traits_type::not_eof(__c);
}

template <class _CharT, class _Traits>
std::basic_streambuf<_CharT, _Traits>*
basic_curlbuf<_CharT, _Traits>::setbuf(char_type *__s, std::streamsize __n)
{
    this->setg(0, 0, 0);
    this->setp(0, 0);
    if (__owns_eb_)
        delete [] __extbuf_;
    if (__owns_ib_)
        delete [] __intbuf_;
    __ebs_ = __n;
    if (__ebs_ > sizeof(__extbuf_min_))
    {
        if (__always_noconv_ && __s)
        {
            __extbuf_ = (char*)__s;
            __owns_eb_ = false;
        }
        else
        {
            __extbuf_ = new char[__ebs_];
            __owns_eb_ = true;
        }
    }
    else
    {
        __extbuf_ = __extbuf_min_;
        __ebs_ = sizeof(__extbuf_min_);
        __owns_eb_ = false;
    }
    if (!__always_noconv_)
    {
        __ibs_ = std::max<std::streamsize>(__n, sizeof(__extbuf_min_));
        if (__s && __ibs_ >= sizeof(__extbuf_min_))
        {
            __intbuf_ = __s;
            __owns_ib_ = false;
        }
        else
        {
            __intbuf_ = new char_type[__ibs_];
            __owns_ib_ = true;
        }
    }
    else
    {
        __ibs_ = 0;
        __intbuf_ = 0;
        __owns_ib_ = false;
    }
    return this;
}

template <class _CharT, class _Traits>
typename basic_curlbuf<_CharT, _Traits>::pos_type
basic_curlbuf<_CharT, _Traits>::seekoff(off_type __off, std::ios_base::seekdir __way, std::ios_base::openmode)
{
    // can't seek on remote resources
    return pos_type(off_type(-1));
}

template <class _CharT, class _Traits>
typename basic_curlbuf<_CharT, _Traits>::pos_type
basic_curlbuf<_CharT, _Traits>::seekpos(pos_type __off, std::ios_base::openmode)
{
    // can't seek on remote resources
    return pos_type(off_type(-1));
}

template <class _CharT, class _Traits>
int
basic_curlbuf<_CharT, _Traits>::sync()
{
    if (__curl_ == 0)
        return 0;
    if (__cm_ & std::ios_base::out)
    {
        if (this->pptr() != this->pbase())
            if (overflow() == traits_type::eof())
                return -1;
        std::codecvt_base::result __r;
        do
        {
            char* __extbe;
            __r = __cv_->unshift(__st_, __extbuf_, __extbuf_ + __ebs_, __extbe);
            size_t __nmemb = static_cast<size_t>(__extbe - __extbuf_);
            size_t __nsent = 0;
            if ( curl_easy_send(__curl_, __extbuf_, __nmemb*sizeof(char_type), &__nsent) != CURLE_OK || __nsent != __nmemb*sizeof(char_type) )
                return -1;
        } while (__r == std::codecvt_base::partial);
        if (__r == std::codecvt_base::error)
            return -1;
    }
    else if (__cm_ & std::ios_base::in)
    {
        off_type __c;
        if (__always_noconv_)
            __c = this->egptr() - this->gptr();
        else
        {
            int __width = __cv_->encoding();
            __c = __extbufend_ - __extbufnext_;
            if (__width > 0)
                __c += __width * (this->egptr() - this->gptr());
            else
            {
                if (this->gptr() != this->egptr())
                {
                    std::reverse(this->gptr(), this->egptr());
                    std::codecvt_base::result __r;
                    const char_type* __e = this->gptr();
                    char* __extbe;
                    do
                    {
                        __r = __cv_->out(__st_, __e, this->egptr(), __e,
                                         __extbuf_, __extbuf_ + __ebs_, __extbe);
                        switch (__r)
                        {
                            case std::codecvt_base::noconv:
                                __c += this->egptr() - this->gptr();
                                break;
                            case std::codecvt_base::ok:
                            case std::codecvt_base::partial:
                                __c += __extbe - __extbuf_;
                                break;
                            default:
                                return -1;
                        }
                    } while (__r == std::codecvt_base::partial);
                }
            }
        }
        this->setg(0, 0, 0);
        __cm_ = 0;
    }
    return 0;
}

template <class _CharT, class _Traits>
void
basic_curlbuf<_CharT, _Traits>::imbue(const std::locale& __loc)
{
    sync();
    __cv_ = &std::use_facet<std::codecvt<char_type, char, state_type> >(__loc);
    bool __old_anc = __always_noconv_;
    __always_noconv_ = __cv_->always_noconv();
    if (__old_anc != __always_noconv_)
    {
        this->setg(0, 0, 0);
        this->setp(0, 0);
        // invariant, char_type is char, else we couldn't get here
        if (__always_noconv_)  // need to dump __intbuf_
        {
            if (__owns_eb_)
                delete [] __extbuf_;
            __owns_eb_ = __owns_ib_;
            __ebs_ = __ibs_;
            __extbuf_ = (char*)__intbuf_;
            __ibs_ = 0;
            __intbuf_ = 0;
            __owns_ib_ = false;
        }
        else  // need to obtain an __intbuf_.
        {     // If __extbuf_ is user-supplied, use it, else new __intbuf_
            if (!__owns_eb_ && __extbuf_ != __extbuf_min_)
            {
                __ibs_ = __ebs_;
                __intbuf_ = (char_type*)__extbuf_;
                __owns_ib_ = false;
                __extbuf_ = new char[__ebs_];
                __owns_eb_ = true;
            }
            else
            {
                __ibs_ = __ebs_;
                __intbuf_ = new char_type[__ibs_];
                __owns_ib_ = true;
            }
        }
    }
}

template <class _CharT, class _Traits>
bool
basic_curlbuf<_CharT, _Traits>::__read_mode()
{
    if (!(__cm_ & std::ios_base::in))
    {
        this->setp(0, 0);
        if (__always_noconv_)
            this->setg((char_type*)__extbuf_,
                       (char_type*)__extbuf_ + __ebs_,
                       (char_type*)__extbuf_ + __ebs_);
        else
            this->setg(__intbuf_, __intbuf_ + __ibs_, __intbuf_ + __ibs_);
        __cm_ = std::ios_base::in;
        return true;
    }
    return false;
}

template <class _CharT, class _Traits>
void
basic_curlbuf<_CharT, _Traits>::__write_mode()
{
    if (!(__cm_ & std::ios_base::out))
    {
        this->setg(0, 0, 0);
        if (__ebs_ > sizeof(__extbuf_min_))
        {
            if (__always_noconv_)
                this->setp((char_type*)__extbuf_,
                           (char_type*)__extbuf_ + (__ebs_ - 1));
            else
                this->setp(__intbuf_, __intbuf_ + (__ibs_ - 1));
        }
        else
            this->setp(0, 0);
        __cm_ = std::ios_base::out;
    }
}

template <class _CharT, class _Traits>
template <typename... Args>
bool
basic_curlbuf<_CharT, _Traits>::SetOption(CURLoption opt, const Args&... args)
{
    if ( __curl_ == nullptr )
        return false;
    
    return curl_easy_setopt(__curl_, opt, args...);
}

template <class _CharT, class _Traits>
bool
basic_curlbuf<_CharT, _Traits>::CanHandleURLScheme(const std::string &url)
{
    auto loc = url.find_first_of(':');
    if ( loc == std::string::npos )
        return false;
    std::string __tmp(url, 0, loc);
    if ( __curl_schemes_.find(__tmp) != __curl_schemes_.end() )
        return true;
    return false;
}

typedef basic_curlbuf<char>     curlbuf;
typedef basic_curlbuf<wchar_t>  wcurlbuf;

template <class _CharT, class _Traits>
const std::map<std::string, bool> basic_curlbuf<_CharT, _Traits>::__curl_schemes_ = {
    { "dict", true },
    { "file", true },
    { "ftp", true },
    { "ftps", true },
    { "gopher", true },
    { "http", true },
    { "https", true },
    { "imap", true },
    { "imaps", true },
    { "ldap", true },
    { "ldaps", true },
    { "pop3", true },
    { "pop3s", true },
    { "pop", true },
    { "rtmp", true },
    { "rtsp", true },
    { "scp", true },
    { "sftp", true },
    { "smtp", true },
    { "smtps", true },
    { "telnet", true },
    { "tftp", true }
};

EPUB3_END_NAMESPACE

#endif
