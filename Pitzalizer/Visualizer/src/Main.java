import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

public class Main {
    final static int WIDTH = 1000;
    final static int HEIGHT = 1000;
    final static int FIRST_PHASE_TURNS = 105;
    static final String[] names = new String[3];
    static int ALL_TURNS;
    static int n, m;
    static List<Integer>[] graph;
    static List<Put>[] puted;
    static List<Attack>[] attacks;
    static int[] score, finalRate = new int[3];
    static double[] x, y;
    static int[][] owner, defense, all;
    static Move[] moves;
    static Scanner scanner = new Scanner(System.in);

    static void get_score() {
        score = new int[n];
        for (int i = 0; i < n; i++) score[i] = 0;
        for (int i = 0; i < 6; i++) {
            int node = scanner.nextInt();
            int scoreNode = scanner.nextInt();
            score[node] = scoreNode;
        }
    }

    static void get_location() {
        x = new double[n];
        y = new double[n];
        for (int i = 0; i < n; i++) {
            x[i] = scanner.nextDouble();
            y[i] = scanner.nextDouble();
        }
    }

    static void get_graph() {
        graph = new List[n];
        for (int i = 0; i < n; i++) graph[i] = new ArrayList<Integer>();
        for (int u, v, i = 0; i < m; i++) {
            u = scanner.nextInt();
            v = scanner.nextInt();
            graph[u].add(v);
            graph[v].add(u);
        }
    }

    static void get_turn_details() {
        ALL_TURNS = scanner.nextInt();
        owner = new int[ALL_TURNS + 1][n];
        defense = new int[ALL_TURNS + 1][n];
        all = new int[ALL_TURNS + 1][n];
        moves = new Move[ALL_TURNS + 1];
        for (int i = 0; i <= ALL_TURNS; i++)
            for (int j = 0; j < n; j++) {
                defense[i][j] = all[i][j] = 0;
                owner[i][j] = -1;
            }

        puted = new List[ALL_TURNS + 1];
        for (int i = 0; i <= ALL_TURNS; i++)
            puted[i] = new ArrayList<>();

        attacks = new List[ALL_TURNS + 1];
        for (int i = 0; i <= ALL_TURNS; i++)
            attacks[i] = new ArrayList<>();

        int count_puts = scanner.nextInt();
        int[] user = new int[count_puts], node = new int[count_puts];
        for (int i = 0; i < count_puts; i++) {
            user[i] = scanner.nextInt();
            node[i] = scanner.nextInt();
        }
        for (int _turn = 1, i = 0; i < count_puts; i++, _turn++) {
            while (user[i] != _turn % 3) _turn++;
            owner[_turn][node[i]] = _turn % 3;
            all[_turn][node[i]] = 1;
            puted[_turn].add(new Put(node[i], 1));
        }
        for (int j = 0; j < n; j++)
            for (int i = 1; i <= FIRST_PHASE_TURNS; i++) {
                all[i][j] += all[i - 1][j];
                if (all[i][j] > 0 && owner[i][j] == -1)
                    owner[i][j] = owner[i - 1][j];
            }
        for (int i = 0; i <= FIRST_PHASE_TURNS; i++)
            moves[i] = new Move(0, 0, 0);
        for (int i = FIRST_PHASE_TURNS + 1; i <= ALL_TURNS; i++) {
            for (int j = 0; j < n; j++)
                owner[i - 1][j] = scanner.nextInt();
            for (int j = 0; j < n; j++)
                all[i - 1][j] = scanner.nextInt();
            for (int j = 0; j < n; j++) {
                defense[i][j] = scanner.nextInt();
                all[i - 1][j] += defense[i - 1][j];
            }
            int count_puted = scanner.nextInt();
            for (int j = 0; j < count_puted; j++)
                puted[i].add(new Put(scanner.nextInt(), scanner.nextInt()));

            int count_attacks = scanner.nextInt();
            for (int j = 0; j < count_attacks; j++)
                attacks[i].add(new Attack(scanner.nextInt(), scanner.nextInt(), scanner.nextInt(), scanner.nextInt(), scanner.nextInt(), scanner.nextInt()));
            moves[i] = new Move(scanner.nextInt(), scanner.nextInt(), scanner.nextInt());
        }
    }

    static void fill(int turn) {
        System.arraycopy(owner[turn - 1], 0, owner[turn], 0, n);
        System.arraycopy(all[turn - 1], 0, all[turn], 0, n);
        for (Put put : puted[turn]) {
            owner[turn][put.node()] = turn % 3;
            all[turn][put.node()] += put.count();
        }
        for (Attack attack : attacks[turn]) {
            owner[turn][attack.to()] = attack.ownerTo();
            all[turn][attack.to()] = defense[turn][attack.to()] + attack.countTo();
            all[turn][attack.from()] = defense[turn][attack.from()] + attack.countFrom();
        }
        all[turn][moves[turn].end()] += moves[turn].count();
        all[turn][moves[turn].start()] -= moves[turn].count();
    }

    public static void main(String[] args) {
        n = scanner.nextInt();
        m = scanner.nextInt();
        get_score();
        get_location();
        get_graph();
        get_turn_details();
        for (int i = 0; i < 3; i++) finalRate[i] = scanner.nextInt();
        for (int i = 0; i < 3; i++) names[i] = scanner.next();
        //for(int i = 1; i <= ALL_TURNS; i++)
        //    fill(i);
        fill(ALL_TURNS);
        new MyFrame();
    }

    public static int count_puted(int turn, int node) {
        int res = 0;
        for (Put put : puted[turn])
            if (put.node() == node)
                res += put.count();
        return res;
    }

}