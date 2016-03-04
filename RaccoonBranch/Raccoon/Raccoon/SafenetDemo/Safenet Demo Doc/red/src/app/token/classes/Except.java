/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package token.classes;

/**
 *
 * @author mehdi
 */
public final class Except extends Exception {

    // Exception codes
    
    public final static int E_ClassFormatError                  = 0xC1;
    public final static int E_UnsupportedOperationException     = 0xC2;
    public final static int E_ArrayIndexOutOfBoundsException    = 0xC3;
    public final static int E_StringIndexOutOfBoundsException   = 0xC4;
    public final static int E_IllegalArgumentException          = 0xC5;
    public final static int E_NullPointerException              = 0xC6;
    public final static int E_NegativeArraySizeException        = 0xC7;
    public final static int E_OutOfMemoryError                  = 0xC8;
    public final static int E_StackOverflowError                = 0xC9;
    public final static int E_ArithmeticException               = 0xCA;
    public final static int E_IllegalStateException             = 0xCB;
    public final static int E_InternalError                     = 0xCC;
    public final static int E_RuntimeException                  = 0xCD;
    public final static int E_SecurityException                 = 0xCE;
    public final static int E_LicenseExpirated                  = 0xCF;
    
    //public final static int E_CustomExceptionFirst              = 0x40;
    //public final static int E_CustomExceptionLast               = 0x7F;
    
    private Except() {} // do not make instances
    
    public native static RuntimeException genException(int code, String msg);
           
    public native int getCode();
    public native static ArrayIndexOutOfBoundsException 
        ArrayIndexOutOfBoundsException(String msg);
    public native static ArithmeticException 
        ArithmeticException(String msg);
    public native static ClassFormatError 
        ClassFormatError(String msg);
    public native static IllegalArgumentException 
        IllegalArgumentException(String msg);
    public native static IllegalStateException 
        IllegalStateException(String msg);
    public native static InternalError 
        InternalError(String msg);
    public native static NullPointerException 
        NullPointerException(String msg);
    public native static NegativeArraySizeException 
        NegativeArraySizeException(String msg);
    public native static OutOfMemoryError 
        OutOfMemoryError(String msg);
 //   public native static RuntimeException
   //     RuntimeException(String msg);
    public native static SecurityException 
        SecurityException(String msg);
    public native static StackOverflowError 
        StackOverflowError(String msg);
    public native static StringIndexOutOfBoundsException 
        StringIndexOutOfBoundsException(String msg);
    public native static UnsupportedOperationException 
        UnsupportedOperationException(String msg);
    public native static VirtualMachineError 
        VirtualMachineError(String msg);
}
