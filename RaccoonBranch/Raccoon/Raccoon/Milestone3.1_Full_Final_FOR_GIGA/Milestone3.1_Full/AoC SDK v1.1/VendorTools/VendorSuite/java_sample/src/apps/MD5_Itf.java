/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package apps;

public class MD5_Itf {

    public static void MD5Hash(String s, byte[] digest) {
        MD5 hash = new MD5();
        hash.update(s.getBytes(),0,s.length());
        hash.md5final(digest);
    }
}
