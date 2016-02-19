package com.jackpf.kitchensync;

import org.apache.commons.lang3.ArrayUtils;
import org.apache.commons.lang3.StringUtils;
import org.apache.commons.lang3.tuple.ImmutablePair;
import org.apache.commons.lang3.tuple.Pair;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * Created by jackfarrelly on 24/01/2016.
 */
public class Executor {
    private String file;
    private String[] permanentArgs;

    private String escape(String s) {
        return s.replaceAll(" ", "\\ ");
    }

    private String[] escape(String[] sArray) {
        String[] r = new String[sArray.length];

        for (int i = 0; i < sArray.length; i++) {
            r[i] = escape(sArray[i]);
        }

        return r;
    }

    public Executor(String file) {
        this(file, new String[]{});
    }

    public Executor(String file, String[] permanentArgs) /*throws Exception*/ {
        this.file = escape(file);
        this.permanentArgs = escape(permanentArgs);

        /*Process p = Runtime.getRuntime().exec("which " + file);
        p.waitFor();
        BufferedReader reader = new BufferedReader(new InputStreamReader(p.getInputStream()));
        if (reader.readLine() == null) {
            throw new RuntimeException("Could not find ffmpeg binary, is it installed?");
        }*/
    }

    protected String[] buildCmd(String[] args) {
        String[] allArgs = ArrayUtils.addAll(permanentArgs, args);
        String[] cmdArgs = new String[allArgs.length + 1];

        cmdArgs[0] = file;
        for (int i = 0; i < allArgs.length; i++) {
            cmdArgs[i + 1] = escape(allArgs[i]);
        }

        return cmdArgs;
    }

    protected List<String> getStreamOutput(InputStream stream) throws IOException {
        BufferedReader reader = new BufferedReader(new InputStreamReader(stream));
        String line;
        List<String> lines = new ArrayList<>();

        while ((line = reader.readLine())!= null) {
            System.out.println("Output (" + stream.getClass().getName() + "): " + line);

            lines.add(line);
        }

        return lines;
    }

    public Pair<List<String>, List<String>> run(String[] args) throws IOException, InterruptedException {
        String[] cmdArgs = buildCmd(args);

        System.out.println("Executing: " + Arrays.toString(cmdArgs));

        Process p = Runtime.getRuntime().exec(cmdArgs);
        int r = p.waitFor();

        List<String> stdLines = getStreamOutput(p.getInputStream()),
            errLines = getStreamOutput(p.getErrorStream());

        if (r != 0) {
            throw new IOException("Process returned non-zero exit code(" + r + "): " + StringUtils.join(errLines.toArray(), "\n"));
        }

        return new ImmutablePair<>(stdLines, errLines);
    }
}
