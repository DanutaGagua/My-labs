import jssc.SerialPort;
import jssc.SerialPortEvent;
import jssc.SerialPortEventListener;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.util.Vector;

public class Main extends JFrame {

    private static JTextArea inputField, outputField, serviceField;

    private static String currentInputText = "";

    JComboBox parity, dataBits, stopBits, speed, error, destinationAddressBox, sourceAddressBox;

    private static Button buttonNewConnect;

    String[] parityValues = new String[]{"NONE", "EVEN", "MARK", "ODD", "SPACE"};
    String[] dataBitsValues = new String[]{"5", "6", "7", "8"};
    String[] stopBitsValues = new String[]{"1", "1.5", "2"};
    String[] speedValues = new String[]{"50", "75", "100", "150", "300", "600", "1200", "2400", "4800", "9600", "19200",
            "38400", "57600", "115200"};
    String[] errorValues = new String[]{"Yes", "No"};

    private static PortService serialPort;

    static byte sourceAddress = -128, destinationAddress = -128;

    int counter = 0;

    private static final char[] HEX_ARRAY = "0123456789ABCDEF".toCharArray();

    public static String bytesToHex(byte[] bytes) {
        char[] hexChars = new char[bytes.length * 2];
        for (int j = 0; j < bytes.length; j++) {
            int v = bytes[j] & 0xFF;
            hexChars[j * 2] = HEX_ARRAY[v >>> 4];
            hexChars[j * 2 + 1] = HEX_ARRAY[v & 0x0F];
        }
        return new String(hexChars);
    }

    public Main(String portName) {
        super(portName);
        setDefaultCloseOperation(EXIT_ON_CLOSE);

        setInterface(portName);

        connect(portName);

        serviceField.setText("Enter different Destination and Source Address before chating.");
    }

    private static class PortReader implements SerialPortEventListener {
        public void serialEvent(SerialPortEvent event) {
            if (event.isERR()) {
                serviceField.setText("Incompatible port settings, to fix reconnect \nto ports with the same params.");
                return;
            }

            if (event.isRXCHAR() && event.getEventValue() > 0) {
                try {
                    Package sourcePackage = serialPort.readPackage();
                    sourcePackage.unBitstaff();

                    if (destinationAddress == sourceAddress) {
                        serviceField.setText("Enter different Destination and Source Address before chating. \n Get some package ");
                        return;
                    }

                    if (sourcePackage.getFCS() != 0) {
                        serviceField.setText("Error of transfer of package");
                        sourcePackage.cancelError();
                    }

                    if (sourcePackage.getDestinationAddress() != sourceAddress) {
                        serviceField.setText("Port has gotten foreign package");
                    } else {
                        serviceField.setText("");
                        outputField.setText(outputField.getText() + sourcePackage.getData());
                    }
                } catch (NullPointerException ex) {
                    System.out.println(ex);
                }
            }
        }
    }

    private void setInterface(String portName) {
        JTextField titleInput = createTitle("Input", 40);
        JTextField titleOutput = createTitle("Output", 40);
        JTextField titleService = createTitle("Service", 40);
        JTextField titleError = createTitle("Do you want emulate error?", 35);
        JTextField titleDestinationAddressField = createTitle("Destination Address", 14);
        JTextField titleSourceAddressField = createTitle("Source Address", 14);

        JTextField titleParity = createTitle("Parity", 33);
        JTextField titleDataBits = createTitle("Databits", 36);
        JTextField titleStopBits = createTitle("Stopbits", 35);
        JTextField titleSpeed = createTitle("Speed", 33);

        inputField = getField(9, 40, true);
        outputField = getField(9, 40, false);
        serviceField = getField(2, 40, false);

        initComboBoxes();

        buttonNewConnect = new Button("restart connection");
        buttonNewConnect.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                connect(portName);
            }
        });

        JPanel contents = new JPanel();
        contents.add(new JScrollPane(titleInput));
        contents.add(new JScrollPane(inputField));
        contents.add(new JScrollPane(titleOutput));
        contents.add(new JScrollPane(outputField));
        contents.add(new JScrollPane(titleService));
        contents.add((serviceField));

        contents.add(titleError);
        contents.add(error);

        contents.add(titleDestinationAddressField);
        contents.add(destinationAddressBox);

        contents.add(titleSourceAddressField);
        contents.add(sourceAddressBox);

        contents.add(titleParity);
        contents.add(parity);
        contents.add(titleDataBits);
        contents.add(dataBits);
        contents.add(titleStopBits);
        contents.add(stopBits);
        contents.add(titleSpeed);
        contents.add(speed);

        contents.add(buttonNewConnect);

        setContentPane(contents);

        setSize(500, 750);
        setVisible(true);

        inputField.addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent e) {
                processKey(e);
            }

            @Override
            public void keyReleased(KeyEvent e) {
                processKey(e);
            }
        });
    }

    public JTextField createTitle(String name, int columns) {
        JTextField title = new JTextField(name);
        title.setFont(new Font("Dialog", Font.PLAIN, 14));
        title.setColumns(columns);
        title.setEditable(false);

        return title;
    }

    public JTextArea getField(int rows, int columns, boolean isEditable) {
        JTextArea field = new JTextArea(rows, columns);
        field.setFont(new Font("Dialog", Font.PLAIN, 14));
        field.setTabSize(10);
        field.setEditable(isEditable);

        return field;
    }

    public JComboBox getComboBox(String[] values, int index) {
        JComboBox comboBox = new JComboBox(values);
        comboBox.setSelectedIndex(index);

        return comboBox;
    }

    public void initComboBoxes() {
        parity = getComboBox(parityValues, 0);
        dataBits = getComboBox(dataBitsValues, 3);
        stopBits = getComboBox(stopBitsValues, 0);
        speed = getComboBox(speedValues, 9);
        error = getComboBox(errorValues, 1);

        Vector<String> values = new Vector<>();
        for (int i = -128; i <= 127; i++) {
            values.add("" + i);
        }

        destinationAddressBox = new JComboBox(values);
        destinationAddressBox.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                destinationAddress = Byte.parseByte(destinationAddressBox.getSelectedItem().toString());
            }
        });

        sourceAddressBox = new JComboBox(values);
        sourceAddressBox.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                sourceAddress = Byte.parseByte(sourceAddressBox.getSelectedItem().toString());
            }
        });
    }

    private void connect(String portName) {
        if (serialPort != null) {
            serialPort.close();
        }

        try {
            serialPort = new PortService(portName);
        } catch (NullPointerException e) {
            return;
        }

        boolean isOpened = serialPort.open(), flag = false;

        int dataBits = getDataBits();
        int speed = getSpeed();
        int parity = getParity();
        int stopBits = getStopBits();

        if ((dataBits == SerialPort.DATABITS_5) && (stopBits == SerialPort.STOPBITS_2)) flag = true;
        if ((dataBits == SerialPort.DATABITS_6) && (stopBits == SerialPort.STOPBITS_1_5)) flag = true;
        if ((dataBits == SerialPort.DATABITS_7) && (stopBits == SerialPort.STOPBITS_1_5)) flag = true;
        if ((dataBits == SerialPort.DATABITS_8) && (stopBits == SerialPort.STOPBITS_1_5)) flag = true;
        if (flag) {
            serviceField.setText("Incorrect params, can't connect.");
            serialPort.close();
            return;
        }

        serialPort.setParams(parity, dataBits, stopBits, speed);

        serialPort.setFlowControl(SerialPort.FLOWCONTROL_RTSCTS_IN | SerialPort.FLOWCONTROL_RTSCTS_OUT);
        serialPort.addListener(new PortReader());

        if (isOpened) {
            serviceField.setText("Successfully connected.");
        } else {
            serviceField.setText("Can't connect. This port or isn't existing or is opened. \n Restart application.");
        }
    }

    private int getDataBits() {
        try {
            String data = (dataBits.getSelectedItem().toString());

            int value = -1;

            if (data.equals("5")) value = SerialPort.DATABITS_5;
            if (data.equals("7")) value = SerialPort.DATABITS_7;
            if (data.equals("6")) value = SerialPort.DATABITS_6;
            if (data.equals("8")) value = SerialPort.DATABITS_8;

            return value;

        } catch (NullPointerException e) {
            return -1;
        }
    }

    private int getSpeed() {
        try {
            return Integer.parseInt(speed.getSelectedItem().toString());
        } catch (NullPointerException e) {
            return -1;
        }
    }

    private int getParity() {
        try {
            int value;
            String s = parity.getSelectedItem().toString();
            if (s.equals("EVEN")) {
                value = SerialPort.PARITY_EVEN;
            }
            if (s.equals("MARK")) {
                value = SerialPort.PARITY_MARK;
            }
            if (s.equals("NONE")) {
                value = SerialPort.PARITY_NONE;
            }
            if (s.equals("ODD")) {
                value = SerialPort.PARITY_ODD;
            } else {
                value = SerialPort.PARITY_SPACE;
            }

            return value;

        } catch (NullPointerException e) {
            return -1;
        }
    }

    private int getStopBits() {
        try {
            int value = 0;
            String s = stopBits.getSelectedItem().toString();
            if (s.equals("1")) {
                value = SerialPort.STOPBITS_1;
            } else {
                if (s.equals("1.5")) {
                    value = SerialPort.STOPBITS_1_5;
                } else {
                    if (s.equals("2"))
                        value = SerialPort.STOPBITS_2;
                }
            }

            return value;

        } catch (NullPointerException e) {
            return -1;
        }
    }

    private byte getError() {
        try {
            byte value = 0;
            String s = error.getSelectedItem().toString();
            if (s.equals("Yes")) {
                value = 1;
            } else {
                if (s.equals("No")) {
                    value = 0;
                }
            }

            return value;

        } catch (NullPointerException e) {
            return -1;
        }
    }

    private void processKey(KeyEvent e) {
        char symbol = e.getKeyChar();

        String newText = inputField.getText();
        if (!currentInputText.equals(newText) && serialPort != null && newText.length() > 0) {
            if (characterIsCorrectlyEntered(newText, symbol, currentInputText)) {
                counter++;
                currentInputText = newText;

                if (counter == 7) {
                    String value = newText.substring(newText.length() - 7);
                    Package destinationPackage = new Package(value, destinationAddress, sourceAddress, getError());
                    destinationPackage.bitstaff();
                    serialPort.writePackage(destinationPackage);

                    serviceField.setText(bytesToHex(destinationPackage.getContent()));
                    counter = 0;
                }
            } else {
                inputField.setText(currentInputText);
                serviceField.setText("You can add only to end string and only standart ascii characters.");
            }
        }
    }

    public boolean characterIsCorrectlyEntered(String newText,
                                               char character,
                                               String currentInputText) {
        if (!((character >= ' ' && character <= '~') || character == '\t' || character == '\n')) {
            return false;
        }

        if (currentInputText.length() > newText.length()) {
            return false;
        }

        for (int i = 0; i < currentInputText.length(); i++) {
            if (currentInputText.charAt(i) != newText.charAt(i)) {
                return false;
            }
        }

        return true;
    }

    public static void main(String[] args) {
        if (args.length > 0) {
            new Main(args[0]);
        } else {
            System.out.println("Didn't found name of port.");
        }
    }
}
