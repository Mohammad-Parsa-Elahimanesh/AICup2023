import javax.swing.*;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;

public class MyFrame extends JFrame {
    final MyPanel panel = new MyPanel(this);
    final Timer timer = new Timer(1000, actionEvent -> {
        panel.addTurn(1);
    });

    MyFrame() {
        super();
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setExtendedState(JFrame.MAXIMIZED_BOTH);
        setContentPane(panel);
        addKeyListener(keyListener());
        setVisible(true);
        addComponentListener(new ComponentAdapter() {
            public void componentResized(ComponentEvent e) {
                if (getWidth() <= 200 && getHeight() <= 200) {
                    setSize(Main.WIDTH, Main.HEIGHT);
                }
            }
        });
    }

    KeyListener keyListener() {
        return new KeyListener() {

            @Override
            public void keyTyped(KeyEvent keyEvent) {
            }

            @Override
            public void keyPressed(KeyEvent keyEvent) {
                switch (keyEvent.getKeyCode()) {
                    case KeyEvent.VK_LEFT -> panel.addTurn(-1);
                    case KeyEvent.VK_RIGHT -> panel.addTurn(1);
                    case KeyEvent.VK_UP -> timer.setDelay(timer.getDelay() / 2 + 60);
                    case KeyEvent.VK_DOWN -> timer.setDelay(Math.min(timer.getDelay() * 2, 20000));
                    case KeyEvent.VK_SPACE -> {
                        if (timer.isRunning()) timer.stop();
                        else timer.start();
                    }
                    case KeyEvent.VK_I -> panel.reverseID();
                }
            }

            @Override
            public void keyReleased(KeyEvent keyEvent) {

            }
        };
    }

}
