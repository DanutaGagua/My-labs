public class Package {
    char flag;
    byte destinationAddress;
    byte sourceAddress;
    char access;
    char data;

    private static final char[] HEX_ARRAY = "0123456789ABCDEF".toCharArray();
    public static String byteToHex(byte bytes) {
        char[] hexChars = new char[2];

        int v = bytes & 0xFF;
        hexChars[0] = HEX_ARRAY[v >>> 4];
        hexChars[1] = HEX_ARRAY[v & 0x0F];

        return new String(hexChars);
    }

    public Package() {
        flag = 't';
    }

    public String getContentInfo() {
       String string = "" + flag + access + data + byteToHex(destinationAddress) + byteToHex(sourceAddress);
       return string;
    }

    public void setContent(char flag, byte destinationAddress, byte sourceAddress, char access, char data){
        this.flag = flag;
        this.destinationAddress = destinationAddress;
        this.sourceAddress = sourceAddress;
        this.access = access;
        this.data = data;
    }
}