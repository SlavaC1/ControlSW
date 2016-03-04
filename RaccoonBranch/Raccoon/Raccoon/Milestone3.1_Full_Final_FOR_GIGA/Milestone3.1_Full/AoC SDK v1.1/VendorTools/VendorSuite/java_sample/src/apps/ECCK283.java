/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package vm.apps;

import token.classes.Crypto;

/**
 *
 * @author pcheng
 */
public class ECCK283 {

    private final static byte[] k283privatekey ={
      (byte)0x04, (byte)0x8B, (byte)0x88, (byte)0x61, (byte)0x07, (byte)0xBB, (byte)0xF3, (byte)0x2C, (byte)0x96, (byte)0x0C, (byte)0x59, (byte)0xDD,
      (byte)0xDE, (byte)0xD3, (byte)0x4D, (byte)0x22, (byte)0x1F, (byte)0x12, (byte)0x9C, (byte)0xFA, (byte)0xD3, (byte)0xB5, (byte)0x29, 0x07, 0x5D, 0x3D, 0x6B, 0x2C,
      (byte)0x11, (byte)0x41, (byte)0x47, (byte)0x6D, (byte)0x1E, (byte)0x75, (byte)0x10, (byte)0x94
            
};
    static {
        Crypto.k283LoadEccKey(k283privatekey);
    }
    

 
    public void K283EccSign(String str,  byte[] signature) {
        byte[] msg = new byte[32];
        //sha256
        Crypto.sha256Init();
        Crypto.sha256Update(str);
        Crypto.sha256Finish(msg);   
        //sign the hash from sha256
        Crypto.k283SignMessage(msg, signature);
    }
    public int K283EccVerify(String str, byte[] signature, byte[] public_key) {
        byte[] msg = new byte[32];
        //sha256
        Crypto.sha256Init();
        Crypto.sha256Update(str);
        Crypto.sha256Finish(msg);       
        
        return Crypto.k283VerifyMessage(msg, signature,public_key);
    }    
}


