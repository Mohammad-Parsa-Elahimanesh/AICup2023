import javax.swing.*;
import java.awt.*;

public class MyPanel extends JPanel {
    final MyFrame parent;
    final Color[] colors = {Color.YELLOW, Color.RED, Color.GREEN, new Color(41, 141, 185)};
    boolean showId = false;
    int turn = 0;

    MyPanel(MyFrame parent) {
        this.parent = parent;
    }

    int getRadius(int node) {
        return (Main.score[node] > 0 ? 50 : 35);
    }

    void addTurn(int x) {
        turn += x;
        if (turn > Main.ALL_TURNS) turn = Main.ALL_TURNS;
        if (turn < 0) turn = 0;
        parent.repaint();
    }

    void reverseID() {
        showId = !showId;
        parent.repaint();
    }

    int okX(double x) {
        return (int) ((x * (getWidth() - 80)) + 40);
    }

    int okY(double y) {
        return (int) ((y * (getHeight() - 120)) + 30);
    }

    @Override
    protected void paintComponent(Graphics gg) {
        super.paintComponent(gg);
        Graphics2D g = (Graphics2D) gg;
        for (int i = 0; i < Main.n; i++) {
            for (Integer u : Main.graph[i])
                if (i < u) {
                    boolean isAttack = false;
                    if (turn != Main.ALL_TURNS)
                        for (Attack attack : Main.attacks[turn + 1])
                            if ((attack.from() == i && attack.to() == u) || (attack.from() == u && attack.to() == i)) {
                                isAttack = true;
                                break;
                            }
                    if (isAttack) {
                        g.setStroke(new BasicStroke(6));
                        g.setColor(colors[(turn + 1) % 3 + 1]);
                    } else {
                        g.setStroke(new BasicStroke(2));
                        g.setColor(new Color(233, 144, 26));
                    }
                    g.drawLine(okX(Main.x[i]), okY(Main.y[i]), okX(Main.x[u]), okY(Main.y[u]));
                }
        }

        for (int i = 0; i < Main.n; i++) {
            int r = getRadius(i);
            int count = (turn < Main.ALL_TURNS ? Main.count_puted(turn + 1, i) : 0);
            int count2 = (turn < Main.ALL_TURNS && (Main.moves[turn + 1].start() == i || Main.moves[turn + 1].end() == i)? Main.moves[turn + 1].count() : 0);
            if (count > 0 || count2 > 0) {
                g.setColor(colors[(turn + 1) % 3 + 1]);
                g.fillOval(okX(Main.x[i]) - r / 2, okY(Main.y[i]) - r / 2, r + 6, r + 6);
                g.setFont(new Font("Arial", Font.BOLD, 17));
                if (count > 0) g.drawString("+" + count, okX(Main.x[i]) - r / 2 - 4, okY(Main.y[i]) - r / 2 - 2);
                if (count2 > 0)
                    g.drawString((Main.moves[turn + 1].start() == i ? "-" : "+") + count2, okX(Main.x[i]) + r / 2 - 4, okY(Main.y[i]) - r / 2 - 2);
            }

            if (turn != Main.ALL_TURNS && Main.defense[turn + 1][i] > Main.defense[turn][i]) {
                g.setColor(Color.black);
                g.fillOval(okX(Main.x[i]) - r / 2 - 2, okY(Main.y[i]) - r / 2 - 2, r + 4, r + 4);
            }
            g.setColor(colors[Main.owner[turn][i] + 1]);
            g.fillOval(okX(Main.x[i]) - r / 2, okY(Main.y[i]) - r / 2, r, r);
            g.setColor(Color.BLACK);
            String s = "" + (Main.all[turn][i] - Main.defense[turn][i]) + "" + (Main.defense[turn][i] > 0 ? "(" + Main.defense[turn][i] + ")" : "");
            g.setFont(new Font("Arial", Font.BOLD, 17));

            if (Main.owner[turn][i] >= 0) g.drawString(s, okX(Main.x[i]) - 14, okY(Main.y[i]) + 7);
            if (showId) {
                g.setFont(new Font("Arial", Font.BOLD, 10));
                g.drawString("id:" + i, okX(Main.x[i]) - 14, okY(Main.y[i]) - 7);
                if (Main.score[i] > 0) {
                    g.drawString("score:" + Main.score[i], okX(Main.x[i]) - 18, okY(Main.y[i]) + 19);
                }
            }

        }


        g.setColor(Color.BLACK);
        g.fillRect(40, getHeight() - 60, getWidth() - 80, 20);
        g.setColor(Color.RED);
        g.fillRect(40 + (getWidth() - 80) * turn / Main.ALL_TURNS, getHeight() - 65, 10, 30);
        g.setColor(colors[(turn + 1) % 3 + 1]);
        g.setFont(new Font("Arial", Font.BOLD, 18));
        g.drawString("Turn: " + turn, 20, getHeight() - 15);
        for (int i = 0; i < 3; i++) {
            g.setColor(colors[i + 1]);
            g.drawString("Score " + Main.names[i] + ": " + Main.finalRate[i], 10, 16 + 22 * i);
        }
    }
}
