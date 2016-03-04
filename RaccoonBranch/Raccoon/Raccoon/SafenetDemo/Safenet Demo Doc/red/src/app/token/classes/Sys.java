/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package token.classes;

/**
 *
 * @author pcheng
 */
public class Sys {

    private Sys() {
    } // do not make instances
    
    public native static byte[] toBytes(short[] data);
    public native static byte[] toBytes(char[] data);
    public native static byte[] toBytes(int[] data);
    public native static void memSet(byte[] dst, int dst_off, int size, byte value);
    public native static void memCpy(byte[] dst, int dst_off, byte[] src, int src_off, int size) ;
    public native static boolean memCmp(byte[] addr1, int off1, byte[] addr2, int off2, int size) ;
    public native static void free(Object obj);
    public native static void free(byte[] data);
    public native static void free(char[] data);
    public native static void free(short[] data);
    public native static void free(int[] data);
    public native static void exit(int rc);
}


