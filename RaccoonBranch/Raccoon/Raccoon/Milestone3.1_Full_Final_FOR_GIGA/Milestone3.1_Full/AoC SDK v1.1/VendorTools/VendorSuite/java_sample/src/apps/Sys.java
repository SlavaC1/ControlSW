package apps;

public class Sys {

	public static void memSet(byte[] in, int p, int i, byte b) {
		for(int j= p; j < p+i;j++)
		{
			in[j] = b;
		}
	}

	public static void memCpy(byte[] in, int i, byte[] newbuf, int bufstart,
			int j) {
		for(int k= i; k < (i+j);k++)
		{
			in[k] = newbuf[bufstart + k];
		}
	}

}
