import javax.swing.*;
import java.awt.*;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;

public class Main extends JFrame {

    private static JTextArea inputField, outputField, serviceField;

    private static String currentInputText = "";

    boolean jam = false, inputEnable = true, packageColision = false, flagSend = true;

    JComboBox packageBox;

    String[] packageValues = new String[] {"Yes", "No"};

    int counter = 0;

    public Main()
    {
        super();
        setDefaultCloseOperation(EXIT_ON_CLOSE);

        setInterface();
    }

    private void setInterface()
    {
        JTextField titleInput = createTitle("Input", 40);
        JTextField titleOutput = createTitle("Output", 40);
        JTextField titleService = createTitle("Service", 40);
        JTextField titleError = createTitle("Do you want to work with package?", 35);

        initFields();
        initComboBoxes();

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

        inputField.addKeyListener(new KeyAdapter()
        {
            @Override
            public void keyPressed(KeyEvent e)
            {
                keyProcess(e);
            }

            @Override
            public void keyReleased(KeyEvent e)
            {
                keyProcess(e);
            }
        });
    }

    public JTextField createTitle(String name, int columns)
    {
        JTextField title = new JTextField(name);
        title.setFont(new Font("Dialog", Font.PLAIN, 14));
        title.setColumns(columns);
        title.setEditable(false);

        return title;
    }

    public void initFields()
    {
        inputField = new JTextArea(8, 40);
        inputField.setFont(new Font("Dialog", Font.PLAIN, 14));
        inputField.setTabSize(10);

        outputField = new JTextArea(8, 40);
        outputField.setFont(new Font("Dialog", Font.PLAIN, 14));
        outputField.setTabSize(10);
        outputField.setEditable(false);

        serviceField = new JTextArea(8, 40);
        serviceField.setFont(new Font("Dialog", Font.PLAIN, 14));
        serviceField.setTabSize(10);
        serviceField.setEditable(false);
    }

    public void initComboBoxes()
    {
        packageBox = new JComboBox(packageValues);
        packageBox.setSelectedIndex(1);
    }

    private byte getPackage() {
        try {
            byte value = 0;
            String s = packageBox.getSelectedItem().toString();
            if (s.equals("Yes")) {
                value = 1;
            }else {
                if (s.equals("No")) {
                    value = 0;
                } }

            return value;

        } catch (NullPointerException e) {
            return -1;
        }
    }

    public void keyProcess(KeyEvent e)
    {
        if (!inputEnable)
        {
            return;
        }

        char symbol = e.getKeyChar();

        int flag = -1;

        String newText = inputField.getText();
        if (!currentInputText.equals(newText) )
        {
            if ((symbol >= ' ' && symbol <= '~') || symbol == '\t' || symbol == '\n')
            {

                if (currentInputText.length() > newText.length())
                {
                    flag = 1;
                }
                else {

                    for (int  i = 0; i < currentInputText.length(); i++)
                    {
                        if (currentInputText.charAt(i) != newText.charAt(i))
                        {
                            flag = i;
                            break;
                        }
                    }

                    if (flag == -1)
                    {
                        currentInputText = newText;

                        if (getPackage() == 1)
                        {
                            counter++;

                            if (counter == 4) {
                                String value = newText.substring(newText.length()-4);

                                inputField.setEditable(false);
                                inputEnable = false;

                                for (int i = 0; i < 4; i++)
                                {
                                    if(i == 0)
                                    {
                                        while(System.currentTimeMillis() % 2 == 0);

                                        sendCharacter(value.charAt(i), i);

                                        if (!flagSend)
                                        {
                                            flagSend = true;
                                            break;
                                        }
                                    }
                                    else {
                                        sendCharacter(value.charAt(i), i);
                                    }
                                }

                                inputField.setEditable(true);
                                inputEnable = true;
                                packageColision = false;

                                counter = 0;
                            }
                        }
                        else
                        {
                            inputField.setEditable(false);
                            inputEnable = false;

                            while(System.currentTimeMillis() % 2 == 0);

                            sendCharacter(newText.charAt(newText.length()-1), 0);

                            flagSend = true;

                            inputField.setEditable(true);
                            inputEnable = true;

                            counter = 0;
                        }
                    }
                }
            }
            else
            {
                flag = 0;
            }

            if (flag > -1)
            {
                inputField.setText(currentInputText);
                serviceField.setText(serviceField.getText() + "You can add only to end string and only standart ascii characters.\n");
            }
        }
    }

    public void sendCharacter(char character, int index)
    {
        int attempt = 0, maxAttempt = 10, slottime = 3;

        char buffer = character;

        long start = System.currentTimeMillis();

        while (System.currentTimeMillis() - start < slottime);

        while (attempt < maxAttempt)
        {
            if (System.currentTimeMillis() % 2 == 0 || packageColision)
            {
                jam = true;

                serviceField.setText(serviceField.getText() + "x");

                long startColision = System.currentTimeMillis();

                while (System.currentTimeMillis() - startColision < (int)Math.pow(2, attempt+1)*slottime);

                jam = false;

                attempt++;

                if (index > 0 || packageColision)
                {
                    packageColision = true;
                    serviceField.setText(serviceField.getText() + "\n");

                    break;
                }
                else

                if (attempt == maxAttempt)
                {
                    serviceField.setText(serviceField.getText() + "\n");

                    flagSend = false;
                }
            }
            else
            {
                outputField.setText(outputField.getText() + buffer);
                serviceField.setText(serviceField.getText() + "\n");

                break;
            }
        }
    }

    public static void main(String[] args)
    {
        new Main();
    }
}