using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Security.Cryptography;
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

        public uint CreateRandomNumber()
        {
            var generator = new System.Security.Cryptography.RNGCryptoServiceProvider();
            byte[] r = {0, 0, 0, 0};
            generator.GetBytes(r);
            return ((uint)(r[0]) | (((uint)(r[1])) << 8) | (((uint)(r[2])) << 16) | (((uint)(r[3])) << 24));
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
