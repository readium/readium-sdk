using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using PhoneSupportInterfaces;

namespace ReadiumPhoneSupport
{
    class XmlDocumentFactory : PhoneSupportInterfaces.IXmlDocumentFactory
    {
        public IXmlLoadSettings CreateLoadSettings()
        {
            return new XmlLoadSettings();
        }

        public IXmlDocument CreateXmlDocument()
        {
            return new XmlDocument();
        }
    };

    class CryptoFactory : PhoneSupportInterfaces.ICryptoFactory
    {
        public ICryptoEngine CreateAES128()
        {
            return new AESCryptoEngine();
        }

        public IHasher CreateSHA1()
        {
            return new SHA1CryptoHasher();
        }

        public IHasher CreateSHA256()
        {
            return new SHA256CryptoHasher();
        }
    }

    public class Initialization
    {
        public static void InitializeFactories()
        {
            FactoryGlue glueObj = FactoryGlue.Singleton();
            glueObj.XmlFactory = new XmlDocumentFactory();
            glueObj.CryptoFactory = new CryptoFactory();
        }
    }
}
