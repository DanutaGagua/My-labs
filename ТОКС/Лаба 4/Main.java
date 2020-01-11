import javax.swing.*;
import java.awt.*;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;

public class Main extends JFrame {

    private static JTextArea inputField, outputField, serviceField;

    private static String currentInputText = "";

    boolean jam = false, inputEnable = true, packageColision = false, flagSend = true;

    JComboBox packageBox;
    String[] packageValues = new String[]{"Yes", "No"};

    int counter = 0;
    static final int packageLength = 4;

    public Main() {
        super();
        setDefaultCloseOperation(EXIT_ON_CLOSE);

        setInterface();
    }

    private void setInterface() {
        JTextField titleInput = createTitle("Input", 40);
        JTextField titleOutput = createTitle("Output", 40);
        JTextField titleService = createTitle("Service", 40);
        JTextField titleError = createTitle("Do you want to work with package?", 35);

        inputField = getField(8, 40, true);
        outputField = getField(8, 40, false);
        serviceField = getField(8, 40, false);

        packageBox = new JComboBox(packageValues);
        packageBox.setSelectedIndex(1);

        JPanel contents = new JPanel();
        contents.add(new JScrollPane(titleInput));
        contents.add(new JScrollPane(inputField));
        contents.add(new JScrollPane(titleOutput));
        contents.add(new JScrollPane(outputField));
        contents.add(new JScrollPane(titleService));
        contents.add(new JScrollPane(serviceField));
        contents.add(titleError);
        contents.add(packageBox);

        setContentPane(contents);

        setSize(500, 650);
        setVisible(true);

        inputField.addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent e) {
                keyProcess(e);
            }

            @Override
            public void keyReleased(KeyEvent e) {
                keyProcess(e);
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

    private byte getPackage() {
        try {
            byte value = 0;
            String s = packageBox.getSelectedItem().toString();

            if (s.equals("Yes")) {
                value = 1;
            }
            if (s.equals("No")) {
                value = 0;
            }

            return value;

        } catch (NullPointerException e) {
            return -1;
        }
    }

    public void keyProcess(KeyEvent e) {
        if (!inputEnable) {
            System.out.println(inputEnable);
            return;
        }

        char character = e.getKeyChar();
        String newText = inputField.getText();

        if (newText.length() == currentInputText.length()){
            return;
        }

        if (characterIsCorrectlyEntered(newText, character, currentInputText)) {
            currentInputText = newText;

            if (getPackage() == 1) {
                counter++;

                if (counter == packageLength) {
                    String value = newText.substring(newText.length() - 4);

                    inputField.setEditable(false);
                    inputEnable = false;

                    for (int i = 0; i < 4; i++) {
                        if (i == 0) {
                            while (System.currentTimeMillis() % 2 == 0) ;

                            sendCharacter(value.charAt(i), i);

                            if (!flagSend) {
                                flagSend = true;
                                break;
                            }
                        } else {
                            sendCharacter(value.charAt(i), i);
                        }
                    }

                    inputField.setEditable(true);
                    inputEnable = true;
                    packageColision = false;

                    counter = 0;
                }
            } else {
                inputField.setEditable(false);
                inputEnable = false;

                while (System.currentTimeMillis() % 2 == 0) ;

                sendCharacter(newText.charAt(newText.length() - 1), 0);

                flagSend = true;

                inputField.setEditable(true);
                inputEnable = true;

                counter = 0;
            }
        } else {
            inputField.setText(currentInputText);
            serviceField.setText(serviceField.getText() + "You can add only to end string and only standart ascii characters.\n");
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

    public void sendCharacter(char character, int index) {
        int attempt = 0, maxAttempt = 10, slotTime = 3;

        char buffer = character;

        long start = System.currentTimeMillis();
        while (System.currentTimeMillis() - start < slotTime) ;

        while (attempt < maxAttempt) {
            if (System.currentTimeMillis() % 2 == 0 || packageColision) {
                processColision(attempt, slotTime);
                attempt++;

                if (index > 0 || packageColision) {
                    packageColision = true;
                    serviceField.setText(serviceField.getText() + "\n");

                    break;
                } else if (attempt == maxAttempt) {
                    serviceField.setText(serviceField.getText() + "\n");

                    flagSend = false;
                }
            } else {
                outputField.setText(outputField.getText() + buffer);
                serviceField.setText(serviceField.getText() + "\n");

                break;
            }
        }
    }

    public void processColision(int attempt, int slotTime) {
        jam = true;

        serviceField.setText(serviceField.getText() + "x");

        long startColision = System.currentTimeMillis();
        while (System.currentTimeMillis() - startColision < (int) Math.pow(2, attempt + 1) * slotTime) ;

        jam = false;
    }

    public static void main(String[] args) {
        new Main();
    }
}
