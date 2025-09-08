import org.json.JSONObject;
import org.json.JSONTokener;
import java.io.FileReader;
import java.math.BigDecimal;
import java.math.BigInteger;
import java.math.MathContext;
import java.util.*;

public class PolySolver {

    private static final MathContext MC = MathContext.DECIMAL128;

    // Convert base-N string to decimal BigInteger
    public static BigInteger decode(String value, int base) {
        return new BigInteger(value.toLowerCase(), base);
    }

    // Solve Ax = b using Gaussian elimination with BigDecimal
    public static BigDecimal[] solve(BigDecimal[][] A, BigDecimal[] b) {
        int n = A.length;

        for (int i = 0; i < n; i++) {
            // Pivot
            int max = i;
            for (int j = i + 1; j < n; j++) {
                if (A[j][i].abs().compareTo(A[max][i].abs()) > 0) {
                    max = j;
                }
            }
            // Swap rows
            BigDecimal[] temp = A[i]; A[i] = A[max]; A[max] = temp;
            BigDecimal t = b[i]; b[i] = b[max]; b[max] = t;

            // Check for singularity
            if (A[i][i].compareTo(BigDecimal.ZERO) == 0) {
                throw new ArithmeticException("Matrix is singular or nearly singular");
            }

            // Normalize pivot row
            BigDecimal pivot = A[i][i];
            for (int k = i; k < n; k++) {
                A[i][k] = A[i][k].divide(pivot, MC);
            }
            b[i] = b[i].divide(pivot, MC);

            // Eliminate other rows
            for (int j = 0; j < n; j++) {
                if (j != i) {
                    BigDecimal factor = A[j][i];
                    for (int k = i; k < n; k++) {
                        A[j][k] = A[j][k].subtract(factor.multiply(A[i][k], MC), MC);
                    }
                    b[j] = b[j].subtract(factor.multiply(b[i], MC), MC);
                }
            }
        }
        return b;
    }

    public static void main(String[] args) throws Exception {
        // Load JSON
        String filePath = args.length > 0 ? args[0] : "input.json";
        JSONObject obj = new JSONObject(new JSONTokener(new FileReader(filePath)));
        int k = obj.getJSONObject("keys").getInt("k");
        int degree = k - 1;

        // Collect points
        List<Integer> xs = new ArrayList<>();
        List<BigInteger> ys = new ArrayList<>();
        for (String key : obj.keySet()) {
            if (key.equals("keys")) continue;
            try {
                int x = Integer.parseInt(key);
                JSONObject val = obj.getJSONObject(key);
                int base = Integer.parseInt(val.getString("base"));
                BigInteger y = decode(val.getString("value"), base);
                xs.add(x);
                ys.add(y);
            } catch (Exception e) {
                System.err.println("Invalid key or value in JSON: " + key);
                return;
            }
        }

        // Sort points by x
        List<Integer> indices = new ArrayList<>();
        for (int i = 0; i < xs.size(); i++) indices.add(i);
        indices.sort(Comparator.comparingInt(xs::get));

        // Use first k points
        BigDecimal[][] A = new BigDecimal[k][k];
        BigDecimal[] b = new BigDecimal[k];
        for (int i = 0; i < k; i++) {
            int x = xs.get(indices.get(i));
            BigInteger y = ys.get(indices.get(i));
            b[i] = new BigDecimal(y, MC);
            for (int j = 0; j <= degree; j++) {
                A[i][j] = BigDecimal.valueOf(x).pow(degree - j, MC);
            }
        }

        BigDecimal[] coeffs = solve(A, b);

        // Print only constant term (last coefficient)
        System.out.println(coeffs[coeffs.length - 1].toPlainString());
    }
}