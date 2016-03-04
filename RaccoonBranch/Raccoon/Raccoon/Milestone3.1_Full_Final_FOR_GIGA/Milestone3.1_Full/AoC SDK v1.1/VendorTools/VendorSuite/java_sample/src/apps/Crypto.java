/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package token.classes;

/**
 *
 * @author mehdi
 */
public class Crypto {
    
    public final static int AES_KEY_SIZE = 16;
    public final static int ECC_PRIVATE_KEY_SIZE = 21;
    public final static int ECC_PUBLIC_KEY_SIZE = 42;
    public final static int ECC_SIGNATURE_SIZE = 42;
    public final static int SHA1_DIGEST_SIZE = 20;

    public final static int K283_ECC_PRIVATE_KEY_SIZE = 36;
    public final static int K283_ECC_PUBLIC_KEY_SIZE = 72;
    public final static int K283_ECC_SIGNATURE_SIZE = 72;
    public final static int SHA256_DIGEST_SIZE = 32;
    
    
    public native static int  rand();
    public native static void genRandom(byte[] data,int off,int size);
    
    public native static void hashInit();
    public native static void hashUpdate(String str);
    public native static void hashUpdate(byte[] data);
    public native static void hashUpdate(byte[] data,int off,int size);
    public native static void hashFinish(byte[] digest);

    public native static void sha256Init();
    public native static void sha256Update(String str);
    public native static void sha256Update(byte[] data);
    public native static void sha256Update(byte[] data,int off,int size);
    public native static void sha256Finish(byte[] digest);
    
    public native static void loadAesKey(byte[] key);
    public native static void encrypt(byte[] data,int off);
    public native static void encryptXor(byte[] data,int data_off,byte[] xor_data,int xor_off);
    public native static void decrypt(byte[] data,int off);
    public native static void decryptXor(byte[] data,int data_off,byte[] xor_data,int xor_off);
    
    public native static void loadEccKey(byte[] priv_key);
    public native static void calcPublicKey(byte[] public_key);
    public native static void signMessage(byte[] message_hash,byte[] signature);
    public native static int  verifyMessage(byte[] message_hash,byte[] signature, byte[] public_key);
    
    public native static void k283LoadEccKey(byte[] priv_key);
    public native static void k283SignMessage(byte[] message_hash, byte[] signature);
    public native static int  k283VerifyMessage(byte[] message_hash, byte[] signature, byte[] public_key);    
    
    
    public static void genRandom(byte[] data) {
        Crypto.genRandom(data, 0, data.length);
    }
    
    public static void hash(byte[] data,int off,int size,byte[] digest)
    {
        Crypto.hashInit();
        Crypto.hashUpdate(data,off,size);
        Crypto.hashFinish(digest);
    }

    public static void hash(String str,byte[] digest)
    {
        Crypto.hashInit();
        Crypto.hashUpdate(str);
        Crypto.hashFinish(digest);
    }
    
    public static byte[] chapSha1(byte[] chal)
    {
        byte[] digest = new byte[20];
        int size = chal.length;
        Crypto.hashInit();
      
        while (size > 64)  {
            Crypto.hashUpdate(chal, 0, 64);
            size -= 64;
        }
        
        if (size > 0)   {
           Crypto.hashUpdate(chal, 0, size);
        }
        
        Crypto.hashFinish(digest);
        return digest;
    }
 
    public static void sha256(byte[] data,int off,int size,byte[] digest)
    {
        Crypto.sha256Init();
        Crypto.sha256Update(data,off,size);
        Crypto.sha256Finish(digest);
    }

    public static void sha256(String str,byte[] digest)
    {
        Crypto.sha256Init();
        Crypto.sha256Update(str);
        Crypto.sha256Finish(digest);
    }
    
    public static byte[] chapSha256(byte[] chal)
    {
        byte[] digest = new byte[32];
        int size = chal.length;
        Crypto.sha256Init();
      
        while (size > 64)  {
            Crypto.sha256Update(chal, 0, 64);
            size -= 64;
        }
        
        if (size > 0)   {
           Crypto.sha256Update(chal, 0, size);
        }
        
        Crypto.sha256Finish(digest);
        return digest;
    }    
    
    
    
    public static void encrypt(byte data[],int off,int size)
    {
        if( size >= 16 )
        {
            Crypto.encrypt(data,off);
        
            while( size >= 32 )
            {
                Crypto.encryptXor(data,off+16,data,off);
                size -= 16;
                off += 16;
            }
            if( size > 16 ) Crypto.encrypt(data, off+size-16);
        }
    }

    public static void decrypt(byte data[],int off,int size)
    {
        if( size >= 16 )
        {
            if( (size & 15) != 0 ) {
                Crypto.decrypt(data, off+size-16);
                size &= ~15;
            }
            
            while( size > 16 )
            {
                size -= 16;
                Crypto.decryptXor(data,off+size,data,off+size-16);
            }
            Crypto.decrypt(data,0);
        }
    }
    
    public static void encryptIV(byte iv[],byte data[],int off,int size)
    {
        if( size >= 16 )
        {
            Crypto.encryptXor(data,off,iv,0);
        
            while( size >= 32 )
            {
                Crypto.encryptXor(data,off+16,data,off);
                size -= 16;
                off += 16;
            }
            if( size > 16 ) Crypto.encrypt(data, off+size-16);
        }
    }

    public static void decryptIV(byte iv[],byte data[],int off,int size)
    {
        if( size >= 16 )
        {
            if( (size & 15) != 0 ) {
                Crypto.decrypt(data, off+size-16);
                size &= ~15;
            }
            
            while( size > 16 )
            {
                size -= 16;
                Crypto.decryptXor(data,off+size,data,off+size-16);
            }
            Crypto.decryptXor(data,0,iv,0);
        }
    }
    
    public static void calcMac(byte[] data,int off,int size,byte[] mac)
    {
        Sys.memSet( mac,0,16,(byte)0 );
        
        while( size >= 16 ) {
            Crypto.encryptXor(mac, 0, data, off);
            off += 16;
            size -= 16;
        }
        
        if( size > 0 ) {
            for( int i = 0; i < 16 && size > 0; size-- )
                mac[i++] ^= data[off++];
            
            Crypto.encrypt( mac,0 );
        }
    }
    
    public static byte[] signData(byte[] hash) {
        byte[] sig = new byte[Crypto.ECC_SIGNATURE_SIZE];
        Crypto.signMessage(hash, sig);
        return sig;
    }
    
    public static byte[] calcPublicKey() {
        byte[] pub_key = new byte[Crypto.ECC_PUBLIC_KEY_SIZE];
        Crypto.calcPublicKey(pub_key);
        return pub_key;
    }        
    
    public static byte[] k283SignData(byte[] msg) {
        byte[] sig = new byte[Crypto.K283_ECC_SIGNATURE_SIZE];
        Crypto.k283SignMessage(msg, sig);
        return sig;
    }
    
    public static int k283VerifyData(byte[] msg,byte[] signature, byte[] public_key) {
        int result;
        result = Crypto.k283VerifyMessage(msg, signature, public_key);
        return result;
    }    
}
