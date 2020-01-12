import java.io.UnsupportedEncodingException;
import java.nio.charset.StandardCharsets;

public class Package {
    byte[] content, data;
    static final int contentLength = 13, dataLength = 7;

    byte flag, sourceAddress, destinationAddress, FCS;
    String polinomial = "10000011";

    public Package(String data, byte destinationAddress, byte sourceAddress, byte FCS) {
        content = new byte[contentLength];
        this.data = new byte[dataLength];

        content[0] = 0b00001110;
        content[1] = destinationAddress;
        content[2] = sourceAddress;

        try {
            for (int i = 0; i < dataLength; i++) {
                content[i + 3] = data.getBytes("UTF-8")[i];
            }
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }

        content[contentLength - 3] = getFCS();

        if (FCS == 1) {
            makeError();
        }

        initData();
    }

    public Package(byte[] content) {
        this.content = new byte[contentLength];
        data = new byte[dataLength];

        this.content = content;

        flag = content[0];
        destinationAddress = content[1];
        sourceAddress = content[2];
        FCS = content[contentLength - 3];

        for (int i = 0; i < dataLength; i++) {
            data[i] = content[i + 3];
        }
    }

    public byte[] getContent() {
        return content;
    }

    public byte getDestinationAddress() {
        return destinationAddress;
    }

    public String getData() {
        return new String(data, StandardCharsets.UTF_8);
    }

    public void bitstaff() {
        String contentString = getBinaryContentString(contentLength - 1, 1);

        contentString = contentString.replaceAll("0000111", "00001111");

        for (int i = 1; i < contentLength; i++) {
            content[i] = getByteFromBinaryString(i, contentString);
        }
    }

    public void unBitstaff() {
        String contentString = getBinaryContentString(contentLength - 1, 1);

        contentString = contentString.replaceAll("00001111", "0000111");

        for (int i = 1; i < contentLength - 2; i++) {
            content[i] = getByteFromBinaryString(i, contentString);
        }

        initData();
    }

    public void initData() {
        flag = content[0];
        destinationAddress = content[1];
        sourceAddress = content[2];
        FCS = content[contentLength - 3];

        for (int i = 0; i < dataLength; i++) {
            data[i] = content[i + 3];
        }

        content[contentLength - 1] = 0;
        content[contentLength - 2] = 0;
    }

    public byte getFCS() {
        String contentString = getBinaryContentString(contentLength - 3, 0);

        contentString = divideString(contentString);

        System.out.println(getByteFromBinaryString(contentString.length() / 8, contentString));

        return getByteFromBinaryString(contentString.length() / 8, contentString);
    }

    public void makeError() {
        String contentString = getBinaryContentString(contentLength - 4, 0);
        System.out.println(contentString);

        int bitIndex = (int) (Math.random() * 80);

        if (contentString.charAt(bitIndex) == '1') {
            contentString = contentString.substring(0, bitIndex) + "0" + contentString.substring(bitIndex + 1);
        } else {
            contentString = contentString.substring(0, bitIndex) + "1" + contentString.substring(bitIndex + 1);
        }

        System.out.println(contentString);

        for (int i = 1; i <= contentLength - 4; i++) {
            content[i - 1] = getByteFromBinaryString(i, contentString);
        }
    }

    public void cancelError() {
        String contentString = getBinaryContentString(contentLength - 3, 0), contentStr = "";

        for (int k = 0; k < 80; k++) {
            contentStr = contentString;

            if (contentStr.charAt(k) == '1') {
                contentStr = contentStr.substring(0, k) + "0" + contentStr.substring(k + 1);
            } else {
                contentStr = contentStr.substring(0, k) + "1" + contentStr.substring(k + 1);
            }

            contentStr = divideString(contentStr);

            if (getByteFromBinaryString(contentStr.length() / 8, contentStr) == 0) {
                if (contentString.charAt(k) == '1') {
                    contentString = contentString.substring(0, k) + "0" + contentString.substring(k + 1);
                } else {
                    contentString = contentString.substring(0, k) + "1" + contentString.substring(k + 1);
                }

                for (int i = 1; i <= contentLength - 3; i++) {
                    content[i - 1] = getByteFromBinaryString(i, contentString);
                }

                break;
            }
        }

        initData();
    }

    public String getBinaryContentString(int endNumber, int beginNumber) {
        String contentString = "";

        for (int i = endNumber; i >= beginNumber; i--) {
            String str = String.format("%8s", Integer.toBinaryString(content[i] & 0xFF)).replace(" ", "0");
            contentString = str + contentString;
        }

        return contentString;
    }

    public byte getByteFromBinaryString(int index, String contentString) {
        int number = 1;
        String value = contentString.substring((index - 1) * 8, index * 8);
        for (int j = 0; j < 8; j++) {
            number *= 2;
            String bit = value.substring(j, j + 1);
            number += Integer.parseInt(bit);
        }

        return (byte) number;
    }

    public String divideString(String contentString){
        for (int i = 0; i <= contentString.length() - 8; i++) {
            if (contentString.charAt(i) == '1') {
                for (int j = 0; j < 8; j++) {
                    if (contentString.charAt(i + j) != polinomial.charAt(j)) {
                        contentString = contentString.substring(0, i + j) + "1" + contentString.substring(i + j + 1);
                    } else {
                        contentString = contentString.substring(0, i + j) + "0" + contentString.substring(i + j + 1);
                    }
                }
            }
        }

        return contentString;
    }
}
