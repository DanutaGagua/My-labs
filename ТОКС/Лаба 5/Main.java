import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.util.*;
import java.util.List;

public class Main {
    JFrame[] window = new JFrame[3];

    private static List<JTextArea> inputFields = new ArrayList<>(3);
    private static List<JTextArea> outputFields = new ArrayList<>(3);
    private static List<JTextArea> serviceFields = new ArrayList<>(3);

    private static List<String> currentInputTexts = new ArrayList<>(3);

    private static List<JComboBox> destinationAddressBoxes = new ArrayList<>(3);
    private static List<JComboBox> sourceAddressBoxes = new ArrayList<>(3);

    private static List<Byte> sourceAddresses = new ArrayList<>(3);
    private static List<Byte> destinationAddresses = new ArrayList<>(3);

    private static List<JButton> startTokenButtons = new ArrayList<>(3);
    private static List<JComboBox> releaseTokens = new ArrayList<>(3);

    private List<ArrayDeque<Character>> buffers = new ArrayList<>(3);

    private boolean releaseToken = false;
    String[] values = new String[]{"No", "Yes"};

    public Main() {
        setInterface();
    }

    private void setInterface() {
        JFrame.setDefaultLookAndFeelDecorated(true);

        for (int i = 0; i < 3; i++) {
            JTextField titleInput = createTitle("Input", 40);
            JTextField titleOutput = createTitle("Output", 40);
            JTextField titleService = createTitle("Service", 40);
            JTextField titleDestinationAddress = createTitle("Destination Address", 34);
            JTextField titleSourceAddress = createTitle("Source Address", 34);
            JTextField titleFreeToken = createTitle("Do you want to release token?", 18);

            initFields(i);
            initKeyListener(inputFields.get(i), serviceFields.get(i), currentInputTexts, i);

            JPanel contents = new JPanel();
            contents.add(titleInput);
            contents.add(new JScrollPane(inputFields.get(i)));
            contents.add(titleOutput);
            contents.add(new JScrollPane(outputFields.get(i)));
            contents.add(titleService);
            contents.add(serviceFields.get(i));

            contents.add(titleDestinationAddress);
            contents.add(destinationAddressBoxes.get(i));

            contents.add(titleSourceAddress);
            contents.add(sourceAddressBoxes.get(i));

            contents.add(startTokenButtons.get(i));
            contents.add(titleFreeToken);
            contents.add(releaseTokens.get(i));

            window[i] = new JFrame();
            window[i].setTitle("Window " + i);
            window[i].setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
            window[i].setContentPane(contents);
            window[i].setSize(500, 600);
            window[i].setVisible(true);
        }
    }

    public JTextField createTitle(String name, int columns) {
        JTextField title = new JTextField(name);
        title.setFont(new Font("Dialog", Font.PLAIN, 14));
        title.setColumns(columns);
        title.setEditable(false);

        return title;
    }

    public void initFields(int index) {
        inputFields.add(index, getField(8, 40, true));
        outputFields.add(index, getField(8, 40, false));
        serviceFields.add(index, getField(3, 40, false));
        currentInputTexts.add(index, "");
        destinationAddresses.add(index, (byte) -128);
        sourceAddresses.add(index, (byte) -128);
        destinationAddressBoxes.add(index, getComboBox(destinationAddresses, index));
        sourceAddressBoxes.add(index, getComboBox(sourceAddresses, index));
        setButtonAndComBox(index);
        buffers.add(index, new ArrayDeque<>(0));
    }

    public JTextArea getField(int rows, int columns, boolean isEditable) {
        JTextArea field = new JTextArea(rows, columns);
        field.setFont(new Font("Dialog", Font.PLAIN, 14));
        field.setTabSize(10);
        field.setEditable(isEditable);

        return field;
    }

    public JComboBox getComboBox(List<Byte> addresses, int index) {
        Vector<String> values = new Vector<>();
        for (int i = -128; i <= 127; i++) {
            values.add("" + i);
        }

        JComboBox addressBox = new JComboBox(values);
        addressBox.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                addresses.set(index, Byte.parseByte(addressBox.getSelectedItem().toString()));
            }
        });

        return addressBox;
    }

    public void setButtonAndComBox(int index) {
        startTokenButtons.add(index, new JButton("start token"));
        startTokenButtons.get(index).addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                if (sourceAddresses.get(0) != sourceAddresses.get(1) &&
                        sourceAddresses.get(1) != sourceAddresses.get(2) &&
                        sourceAddresses.get(0) != sourceAddresses.get(2)) {
                    for (int i = 0; i < 3; i++) {
                        startTokenButtons.get(i).setEnabled(false);
                        destinationAddressBoxes.get(i).setEnabled(false);
                        sourceAddressBoxes.get(i).setEnabled(false);
                        serviceFields.get(i).setText("");
                    }

                    new TokenThread(index).start();
                } else {
                    for (int i = 0; i < 3; i++) {
                        serviceFields.get(i).setText("Set different source addresses");
                    }

                }
            }
        });

        releaseTokens.add(index, new JComboBox(values));
        releaseTokens.get(index).addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                if (releaseTokens.get(index).getSelectedIndex() == 1) {
                    for (int i = 0; i < 3; i++) {
                        releaseTokens.get(i).setSelectedIndex(1);
                    }
                    releaseToken = true;
                } else {
                    for (int i = 0; i < 3; i++) {
                        releaseTokens.get(i).setSelectedIndex(0);
                    }
                    releaseToken = false;
                }
            }
        });
    }

    public void initKeyListener(JTextArea inputField,
                                JTextArea serviceField,
                                List<String> currentInputTexts,
                                int index) {
        inputField.addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent e) {
                keyProcess(e, inputField, serviceField, currentInputTexts, index);
            }

            @Override
            public void keyReleased(KeyEvent e) {
                keyProcess(e, inputField, serviceField, currentInputTexts, index);
            }
        });
    }

    public void keyProcess(KeyEvent e,
                           JTextArea inputField,
                           JTextArea serviceField,
                           List<String> currentInputTexts,
                           int index) {
        char symbol = e.getKeyChar();
        String newText = inputField.getText();

        if (!currentInputTexts.get(index).equals(newText)) {
            if (characterIsCorrectlyEntered(newText, symbol, currentInputTexts.get(index))) {
                currentInputTexts.set(index, newText);
                buffers.get(index).add(symbol);
                if (serviceField.getText().indexOf("Token") != -1) {
                    serviceField.setText(serviceField.getText() + "");
                } else {
                    serviceField.setText("");
                }
            } else {
                inputField.setText(currentInputTexts.get(index));
                if (serviceField.getText().indexOf("Token") != -1) {
                    serviceField.setText(serviceField.getText() + "\nYou can add only to end string and only standart ascii characters.");
                } else {
                    serviceField.setText("You can add only to end string and only standart ascii characters.");
                }
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
        new Main();
    }

    private class TokenThread extends Thread {
        private int index;
        private Package tokenPackage = new Package();

        TokenThread(int index) {
            super();
            this.index = index;
        }

        public void run() {
            while (true) {
                if (tokenPackage.flag == 'f') {
                    if (tokenPackage.destinationAddress == sourceAddresses.get(index)) {
                        outputFields.get(index).setText(outputFields.get(index).getText() + tokenPackage.data);
                        tokenPackage.access = 'c';
                    }
                    if (tokenPackage.sourceAddress == sourceAddresses.get(index)) {
                        if (buffers.get(index).isEmpty() || releaseToken) {
                            tokenPackage.flag = 't';
                            if (releaseToken) {
                                serviceFields.get(index).setText("");
                                changeStation();
                            }
                        } else {
                            updatePackage();
                        }
                    }
                } else {
                    if (!buffers.get(index).isEmpty()) {
                        updatePackage();
                    }
                }

                updateServiceField();

                long start = System.currentTimeMillis();
                while (System.currentTimeMillis() - start < 1000) {
                    if (tokenPackage.flag == 't') {
                        if (!buffers.get(index).isEmpty()) {
                            updatePackage();
                            updateServiceField();
                        }
                    }
                }

                serviceFields.get(index).setText("");
                if (tokenPackage.sourceAddress == sourceAddresses.get(index) && tokenPackage.flag == 'f') {
                    serviceFields.get(index).setText("\nToken is captured.");
                }
                changeStation();
            }
        }

        private void updateServiceField() {
            if (tokenPackage.flag == 'f') {
                serviceFields.get(index).setText(tokenPackage.getContentInfo());
                if (tokenPackage.sourceAddress == sourceAddresses.get(index)) {
                    serviceFields.get(index).setText(serviceFields.get(index).getText() + "\nToken is captured.");
                }
            } else {
                serviceFields.get(index).setText("t");
            }
        }

        private void updatePackage() {
            tokenPackage.setContent('f', destinationAddresses.get(index), sourceAddresses.get(index),
                    ' ', buffers.get(index).removeFirst());
        }

        private void changeStation() {
            if (index == 2) {
                index = 0;
            } else {
                index++;
            }
        }
    }
}
