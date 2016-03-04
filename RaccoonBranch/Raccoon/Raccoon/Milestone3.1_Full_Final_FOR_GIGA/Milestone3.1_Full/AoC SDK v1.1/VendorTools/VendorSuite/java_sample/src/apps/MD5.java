package apps;

/*
 * This code implements the MD5 message-digest algorithm.
 * The algorithm is due to Ron Rivest.  This code was
 * written by Colin Plumb in 1993, no copyright is claimed.
 * This code is in the public domain; do with it what you wish.
 *
 * Equivalent code is available from RSA Data Security, Inc.
 * This code has been tested against that, and is equivalent,
 * except that you don't need to include two pages of legalese
 * with every copy.
 *
 * To compute the message digest of a chunk of bytes, declare an
 * MD5Context structure, pass it to MD5Init, call MD5Update as
 * needed on buffers full of bytes, and then call MD5Final, which
 * will fill a supplied 16-byte array with the digest.
 */
public class MD5 {

    int buf[];	// These were originally unsigned ints.
    // This Java code makes an effort to avoid sign traps.
    // buf[] is where the hash accumulates.
    int bytes;	// This is the count of bits hashed so far.
    byte in[];	// This is a buffer where we stash bytes until we have
    // enough (64) to perform a transform operation.
    int inint[];
    // inint[] used and discarded inside transform(),
    // but why allocate it over and over?
    // (In the C version this is allocated on the stack.)
    public MD5() {
        buf = new int[4];
        // fill the hash accumulator with a seed value
        buf[0] = 0x67452301;
        buf[1] = 0xefcdab89;
        buf[2] = 0x98badcfe;
        buf[3] = 0x10325476;

        // initially, we've hashed zero bits
        bytes = 0;

        in = new byte[64];
        inint = new int[16];
    }

    public void update(byte[] newbuf) {
        update(newbuf, 0, newbuf.length);
    }

    public void update(byte[] newbuf, int length) {
        update(newbuf, 0, length);
    }

    public void update(byte[] newbuf, int bufstart, int buflen) {
        int t;
        int len = buflen;

        // shash old bits value for the "Bytes already in" computation
        // just below.
        t = bytes & 0x3f;

        /* update bitcount */
        /* the C code used two 32-bit ints separately, and carefully
         * ensured that the carry carried.
         * Java has a 64-bit long, which is just what the code really wants.
         */
        bytes += len;

        /* Handle any leading odd-sized chunks */
        /* (that is, any left-over chunk left by last update() */

        if (t != 0) {
            int p = 64 - t;
            if (len < p) {
                Sys.memCpy(in, t, newbuf, bufstart, len);
                return;
            }
            Sys.memCpy(in, t, newbuf, bufstart, p);
            transform();
            bufstart += p;
            len -= p;
        }

        /* Process data in 64-byte chunks */
        while (len >= 64) {
            Sys.memCpy(in, 0, newbuf, bufstart, 64);
            transform();
            bufstart += 64;
            len -= 64;
        }

        /* Handle any remaining bytes of data. */
        /* that is, stash them for the next update(). */
        Sys.memCpy(in, 0, newbuf, bufstart, len);
    }

    /*
     * Final wrapup - pad to 64-byte boundary with the bit pattern
     * 1 0* (64-bit count of bits processed, MSB-first)
     */
    public void md5final(byte[] digest) {
        /* "final" is a poor method name in Java. :v) */
        int count;
        int p;		// in original code, this is a pointer; in this java code
        // it's an index into the array this->in.

  
        p = bytes & 0x3F;

        in[p++] = (byte) 0x80;

  
        count = 64 - p;

  
        if (count < 8) {
  
            Sys.memSet(in, p, count, (byte)0);
            transform();

  
            Sys.memSet(in, 0, 56, (byte)0);
        } else {
  
            Sys.memSet(in, p, count - 8, (byte)0);
        }

        // Could use a PUT_64BIT... func here. This is a fairly
        // direct translation from the C code, where bits was an array
        // of two 32-bit ints.
        PUT_32BIT_LSB_FIRST(in, 56, bytes << 3);
        PUT_32BIT_LSB_FIRST(in, 60, 0);

        transform();
        
        PUT_32BIT_LSB_FIRST(digest, 0, buf[0]);
        PUT_32BIT_LSB_FIRST(digest, 4, buf[1]);
        PUT_32BIT_LSB_FIRST(digest, 8, buf[2]);
        PUT_32BIT_LSB_FIRST(digest, 12, buf[3]);

        /* zero sensitive data */
        /* notice this misses any sneaking out on the stack. The C
         * version uses registers in some spots, perhaps because
         * they care about this.
         */
        bytes = 0;
    }

    /////////////////////////////////////////////////////////////////////
    // Below here ye will only finde private functions                 //
    /////////////////////////////////////////////////////////////////////

    // There must be a way to do these functions that's
    // built into Java, and I just haven't noticed it yet.
    
    private static int F1(int w, int x, int y, int z, int data, int s) {

        w += (z ^ (x & (y ^ z))) + data;
        w = (w << s) | (w >>> (32 - s));
        w += x;
        return w;
    }
    private static int F2(int w, int x, int y, int z, int data, int s) {

        w += (y ^ (z & (x ^ y))) + data;
        w = (w << s) | (w >>> (32 - s));
        w += x;
        return w;
    }
    private static int F3(int w, int x, int y, int z, int data, int s) {

        w += (x ^ y ^ z) + data;
        w = (w << s) | (w >>> (32 - s));
        w += x;
        return w;
    }
    private static int F4(int w, int x, int y, int z, int data, int s) {

        w += (y ^ (x | ~z)) + data;
        w = (w << s) | (w >>> (32 - s));
        w += x;
        return w;
    }

    private void transform() {
        /* load in[] byte array into an internal int array */
        int i;
        //int[] inint = new int[16];

        for (i = 0; i < 16; i++) {
            inint[i] = GET_32BIT_LSB_FIRST(in, 4 * i);
        }

        int a, b, c, d;
        a = buf[0];
        b = buf[1];
        c = buf[2];
        d = buf[3];

        a = F1(a, b, c, d, inint[0] + 0xd76aa478, 7);
        d = F1(d, a, b, c, inint[1] + 0xe8c7b756, 12);
        c = F1(c, d, a, b, inint[2] + 0x242070db, 17);
        b = F1(b, c, d, a, inint[3] + 0xc1bdceee, 22);
        a = F1(a, b, c, d, inint[4] + 0xf57c0faf, 7);
        d = F1(d, a, b, c, inint[5] + 0x4787c62a, 12);
        c = F1(c, d, a, b, inint[6] + 0xa8304613, 17);
        b = F1(b, c, d, a, inint[7] + 0xfd469501, 22);
        a = F1(a, b, c, d, inint[8] + 0x698098d8, 7);
        d = F1(d, a, b, c, inint[9] + 0x8b44f7af, 12);
        c = F1(c, d, a, b, inint[10] + 0xffff5bb1, 17);
        b = F1(b, c, d, a, inint[11] + 0x895cd7be, 22);
        a = F1(a, b, c, d, inint[12] + 0x6b901122, 7);
        d = F1(d, a, b, c, inint[13] + 0xfd987193, 12);
        c = F1(c, d, a, b, inint[14] + 0xa679438e, 17);
        b = F1(b, c, d, a, inint[15] + 0x49b40821, 22);

        a = F2(a, b, c, d, inint[1] + 0xf61e2562, 5);
        d = F2(d, a, b, c, inint[6] + 0xc040b340, 9);
        c = F2(c, d, a, b, inint[11] + 0x265e5a51, 14);
        b = F2(b, c, d, a, inint[0] + 0xe9b6c7aa, 20);
        a = F2(a, b, c, d, inint[5] + 0xd62f105d, 5);
        d = F2(d, a, b, c, inint[10] + 0x02441453, 9);
        c = F2(c, d, a, b, inint[15] + 0xd8a1e681, 14);
        b = F2(b, c, d, a, inint[4] + 0xe7d3fbc8, 20);
        a = F2(a, b, c, d, inint[9] + 0x21e1cde6, 5);
        d = F2(d, a, b, c, inint[14] + 0xc33707d6, 9);
        c = F2(c, d, a, b, inint[3] + 0xf4d50d87, 14);
        b = F2(b, c, d, a, inint[8] + 0x455a14ed, 20);
        a = F2(a, b, c, d, inint[13] + 0xa9e3e905, 5);
        d = F2(d, a, b, c, inint[2] + 0xfcefa3f8, 9);
        c = F2(c, d, a, b, inint[7] + 0x676f02d9, 14);
        b = F2(b, c, d, a, inint[12] + 0x8d2a4c8a, 20);

        a = F3(a, b, c, d, inint[5] + 0xfffa3942, 4);
        d = F3(d, a, b, c, inint[8] + 0x8771f681, 11);
        c = F3(c, d, a, b, inint[11] + 0x6d9d6122, 16);
        b = F3(b, c, d, a, inint[14] + 0xfde5380c, 23);
        a = F3(a, b, c, d, inint[1] + 0xa4beea44, 4);
        d = F3(d, a, b, c, inint[4] + 0x4bdecfa9, 11);
        c = F3(c, d, a, b, inint[7] + 0xf6bb4b60, 16);
        b = F3(b, c, d, a, inint[10] + 0xbebfbc70, 23);
        a = F3(a, b, c, d, inint[13] + 0x289b7ec6, 4);
        d = F3(d, a, b, c, inint[0] + 0xeaa127fa, 11);
        c = F3(c, d, a, b, inint[3] + 0xd4ef3085, 16);
        b = F3(b, c, d, a, inint[6] + 0x04881d05, 23);
        a = F3(a, b, c, d, inint[9] + 0xd9d4d039, 4);
        d = F3(d, a, b, c, inint[12] + 0xe6db99e5, 11);
        c = F3(c, d, a, b, inint[15] + 0x1fa27cf8, 16);
        b = F3(b, c, d, a, inint[2] + 0xc4ac5665, 23);

        a = F4(a, b, c, d, inint[0] + 0xf4292244, 6);
        d = F4(d, a, b, c, inint[7] + 0x432aff97, 10);
        c = F4(c, d, a, b, inint[14] + 0xab9423a7, 15);
        b = F4(b, c, d, a, inint[5] + 0xfc93a039, 21);
        a = F4(a, b, c, d, inint[12] + 0x655b59c3, 6);
        d = F4(d, a, b, c, inint[3] + 0x8f0ccc92, 10);
        c = F4(c, d, a, b, inint[10] + 0xffeff47d, 15);
        b = F4(b, c, d, a, inint[1] + 0x85845dd1, 21);
        a = F4(a, b, c, d, inint[8] + 0x6fa87e4f, 6);
        d = F4(d, a, b, c, inint[15] + 0xfe2ce6e0, 10);
        c = F4(c, d, a, b, inint[6] + 0xa3014314, 15);
        b = F4(b, c, d, a, inint[13] + 0x4e0811a1, 21);
        a = F4(a, b, c, d, inint[4] + 0xf7537e82, 6);
        d = F4(d, a, b, c, inint[11] + 0xbd3af235, 10);
        c = F4(c, d, a, b, inint[2] + 0x2ad7d2bb, 15);
        b = F4(b, c, d, a, inint[9] + 0xeb86d391, 21);

        buf[0] += a;
        buf[1] += b;
        buf[2] += c;
        buf[3] += d;
    }

    private int GET_32BIT_LSB_FIRST(byte[] b, int off) {
        return ((int) (b[off + 0] & 0xff)) |
                ((int) (b[off + 1] & 0xff) << 8) |
                ((int) (b[off + 2] & 0xff) << 16) |
                ((int) (b[off + 3] & 0xff) << 24);
    }

    private void PUT_32BIT_LSB_FIRST(byte[] b, int off, int value) {
        b[off + 0] = (byte) (value & 0xff);
        b[off + 1] = (byte) ((value >> 8) & 0xff);
        b[off + 2] = (byte) ((value >> 16) & 0xff);
        b[off + 3] = (byte) ((value >> 24) & 0xff);
    }
}
