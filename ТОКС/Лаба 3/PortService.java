import jssc.SerialPort;
import jssc.SerialPortEventListener;
import jssc.SerialPortException;

public class PortService {
    private SerialPort port;
    private boolean flagIfOpened;

    public PortService(String name) {
        this.port = new SerialPort(name);
        flagIfOpened = false;
    }

    public boolean writeByte(byte symbol) {
        try {
            return port.writeByte(symbol);
        } catch (SerialPortException e) {
            e.printStackTrace();
            return false;
        }
    }

    public boolean writePackage(Package portPackage)
    {
        try
        {
            return  port.writeBytes(portPackage.getContent());
        }
        catch (SerialPortException e)
        {
            e.printStackTrace();
            return false;
        }
    }

    public byte[] readByte()
    {
        try {
            return port.readBytes(1);
        } catch (SerialPortException e) {
            e.printStackTrace();
            return null;
        }
    }

    public Package readPackage()
    {
        try {
            final byte[] bytes = port.readBytes(Package.contentLength);
            Package portPackage = new Package(bytes);

            return portPackage;

        } catch (SerialPortException e) {
            e.printStackTrace();
            return null;
        }
    }

    public boolean open() {
        try {
            flagIfOpened = port.openPort();
            return flagIfOpened;
        } catch (SerialPortException e) {
            e.printStackTrace();
            return false;
        }
    }

    public boolean close() {
        try {
            if (flagIfOpened) {
                flagIfOpened = false;
                return port.closePort();
            }
            return false;
        } catch (SerialPortException e) {
            e.printStackTrace();
            return false;
        }
    }

    public void setParams(int parity, int dataBits, int stopBits, int speed) {
        try {
            port.setParams(speed, dataBits, stopBits, parity);
        } catch (SerialPortException e) {
            e.printStackTrace();
        }
    }

    public void addListener(SerialPortEventListener listener) {
        try {
            port.setEventsMask(SerialPort.MASK_RXCHAR);
            port.addEventListener(listener);
        }
        catch (SerialPortException ex) {
            ex.printStackTrace();
        }
    }

    public void setFlowControl(int mask) {
        try {
            port.setFlowControlMode(mask);
        } catch (SerialPortException e) {
            e.printStackTrace();
        }
    }

    public boolean isOpen() {
        return flagIfOpened;
    }

    public SerialPort getPort() {
        return port;
    }
}

