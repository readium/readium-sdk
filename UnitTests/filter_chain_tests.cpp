//
//  filter_chain_tests.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-09-09.
//  Copyright (c) 2013 The Readium Foundation and contributors. All rights reserved.
//

#include "../ePub3/ePub/container.h"
#include "../ePub3/ePub/font_obfuscation.h"
#include "../ePub3/ePub/package.h"
#include "../ePub3/ePub/filter_manager.h"
#include "../ePub3/ePub/filter_chain.h"
#include "../ePub3/ePub/filter_chain_byte_stream.h"
#include "../ePub3/ePub/filter_chain_byte_stream_range.h"
#include "../ePub3/utilities/byte_stream.h"
#include "../ePub3/utilities/byte_buffer.h"
#include <atomic>
#include "catch.hpp"

#define EPUB_PATH "TestData/cole-voyage-of-life-20120320.epub"

#define FONT_EPUB_PATH "TestData/wasteland-otf-obf-20120118.epub"
#define FONT_SUBPATH "EPUB/OldStandard-Regular.obf.otf"
#define FONT_MANIFEST_ID "font.OldStandard.regular"

static const char* kROT13Content = R"raw(<?kzy irefvba="1.0" rapbqvat="HGS-8"?>
<ugzy kzyaf="uggc://jjj.j3.bet/1999/kugzy">
    <urnq>
        <zrgn punefrg="hgs-8" />
        <yvax ery="fglyrfurrg" glcr="grkg/pff" uers="../pff/pbyr.pff" />
    </urnq>
    <obql pynff="ersybj">
        <u1>Gur Iblntr bs Yvsr</u1>
        <c><rz>Gur Iblntr bs Yvsr</rz> frevrf, cnvagrq ol Gubznf Pbyr va 1842, vf n frevrf bs
            cnvagvatf gung ercerfrag na nyyrtbel bs gur sbhe fgntrf bs uhzna yvsr: puvyqubbq, lbhgu,
            znaubbq, naq byq ntr.</c>
        <c> Gur cnvagvatf sbyybj n iblntre jub geniryf va n obng ba n evire guebhtu gur zvq-19gu
            praghel Nzrevpna jvyqrearff. Va rnpu cnvagvat, nppbzcnavrq ol n thneqvna natry, gur
            iblntre evqrf gur obng ba gur Evire bs Yvsr. Gur ynaqfpncr, pbeerfcbaqvat gb gur frnfbaf
            bs gur lrne, cynlf n znwbe ebyr va gryyvat gur fgbel.</c>
        <c>Va rnpu cvpgher, gur obng'f qverpgvba bs geniry vf erirefrq sebz gur cerivbhf cvpgher. Va
            puvyqubbq, gur vasnag tyvqrf sebz n qnex pnir vagb n evpu, terra ynaqfpncr. Nf n lbhgu,
            gur obl gnxrf pbageby bs gur obng naq nvzf sbe n fuvavat pnfgyr va gur fxl. Va znaubbq,
            gur nqhyg eryvrf ba cenlre naq eryvtvbhf snvgu gb fhfgnva uvz guebhtu ebhtu jngref naq n
            guerngravat ynaqfpncr. Svanyyl, gur zna orpbzrf byq naq gur natry thvqrf uvz gb urnira
            npebff gur jngref bs rgreavgl.</c>
        <frpgvba vq="onpxtebhaq">
            <u2>Onpxtebhaq</u2>
            <vzt nyg="cbegenvg cvpgher bs Gubznf Pbyr" fep="../vzntrf/cbegenvg.wct" vq="cbegenvg" />
            <c>Gubznf Pbyr vf ertneqrq nf gur sbhaqre bs gur Uhqfba Evire Fpubby, na Nzrevpna neg
                zbirzrag gung sybhevfurq va gur zvq-19gu praghel naq jnf pbaprearq jvgu gur
                ernyvfgvp naq qrgnvyrq cbegenlny bs angher ohg jvgu n fgebat vasyhrapr sebz
                Ebznagvpvfz. Guvf tebhc bs Nzrevpna ynaqfpncr cnvagref jbexrq orgjrra nobhg 1825
                naq 1870 naq funerq n frafr bs angvbany cevqr nf jryy nf na vagrerfg va pryroengvat
                gur havdhr angheny ornhgl sbhaq va gur Havgrq Fgngrf. Gur jvyq, hagnzrq angher sbhaq
                va Nzrevpn jnf ivrjrq nf vgf fcrpvny punenpgre; Rhebcr unq napvrag ehvaf, ohg
                Nzrevpn unq gur hapunegrq jvyqrearff. Nf Pbyr'f sevraq Jvyyvnz Phyyra Oelnag
                frezbavmrq va irefr, fb Pbyr frezbavmrq va cnvag. Obgu zra fnj angher nf Tbq'f jbex
                naq nf n ershtr sebz gur htyl zngrevnyvfz bs pvgvrf. Pbyr pyrneyl vagraqrq gur
                Iblntr bs Yvsr gb or n qvqnpgvp, zbenyvmvat frevrf bs cnvagvatf hfvat gur ynaqfpncr
                nf na nyyrtbel sbe eryvtvbhf snvgu.</c>
            <c>Hayvxr Pbyr’f svefg znwbe frevrf, Gur Pbhefr bs Rzcver, juvpu sbphfrq ba gur fgntrf
                bs pvivyvmngvba nf n jubyr, gur Iblntr bs Yvsr frevrf vf n zber crefbany, Puevfgvna
                nyyrtbel gung vagrecergf ivfhnyyl gur wbhearl bs zna guebhtu sbhe fgntrf bs yvsr:
                vasnapl, lbhgu, znaubbq naq byq ntr. Qbar ba pbzzvffvba, gur svavfurq jbexf
                trarengrq n qvfnterrzrag jvgu gur bjare nobhg n choyvp rkuvovgvba. Va 1842 jura Pbyr
                jnf va Ebzr ur qvq n frpbaq frg bs gur frevrf juvpu ba uvf erghea gb Nzrevpn jnf
                fubja gb nppynvz. Gur svefg frg vf ng gur Zhafba-Jvyyvnzf-Cebpgbe Negf Vafgvghgr va
                Hgvpn, Arj Lbex, gur frpbaq frg vf ng gur Angvbany Tnyyrel bs Neg va Jnfuvatgba,
                Q.P.</c>
        </frpgvba>
    </obql>
</ugzy>
)raw";

using namespace ePub3;

class ROT13Filter : public ePub3::ContentFilter, public PointerType<ROT13Filter>
{
private:
    static bool _IsXHTML(ConstManifestItemPtr item) {
        const string& type = item->MediaType();
        if ( type == "text/html" || type == "application/xhtml+xml" )
            return true;
        return false;
    }
    
public:
    ROT13Filter() : ContentFilter(_IsXHTML) {}
    virtual ~ROT13Filter() {}
    
    static ContentFilterPtr Factory(ConstPackagePtr package) {
        return New();
    }
    
    virtual void* FilterData(FilterContext* context, void* data, size_t len, size_t* outputLen) OVERRIDE;
};

void* ROT13Filter::FilterData(FilterContext* context, void* data, size_t len, size_t* outputLen)
{
    char* characters = reinterpret_cast<char*>(data);
    for ( size_t i = 0; i < len; i++ )
    {
        char ch = characters[i];
        if ( ch >= 65 && ch <= 77 )
            characters[i] = ch + 13;
        else if ( ch >= 78 && ch <= 90 )
            characters[i] = ch - 13;
        else if ( ch >= 97 && ch <= 109 )
            characters[i] = ch + 13;
        else if ( ch >= 110 && ch <= 122 )
            characters[i] = ch - 13;
    }
    
    if (outputLen != nullptr)
        *outputLen = len;
    return data;
}

static void RegisterTestFilter()
{
    std::atomic_flag registered(false);
    if ( registered.test_and_set() == false )
    {
        FilterManager::Instance()->RegisterFilter("ROT13", ContentFilter::ValidationComplete-1, ROT13Filter::Factory);
    }
}

