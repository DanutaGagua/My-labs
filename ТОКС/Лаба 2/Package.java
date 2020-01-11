import java.io.UnsupportedEncodingException;
import java.nio.charset.StandardCharsets;

public class Package {
    byte[] content, data;
    static final int contentLength = 13, dataLength = 7;

    byte flag, sourceAddress, destinationAddress, FCS;

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

        content[contentLength - 3] = FCS;

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
        System.out.println(contentString);

        contentString = contentString.replaceAll("0000111", "00001111");
        System.out.println(contentString);

        for (int i = 1; i < contentLength; i++) {
            content[i] = getByteFromBinaryString(i, contentString);
        }
    }

    public void unBitstaff() {
        String contentString = getBinaryContentString(contentLength - 1, 1);
        System.out.println(contentString);

        contentString = contentString.replaceAll("00001111", "0000111");
        System.out.println(contentString);

        for (int i = 1; i < contentLength - 2; i++) {
            content[i] = getByteFromBinaryString(i, contentString);
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
}
