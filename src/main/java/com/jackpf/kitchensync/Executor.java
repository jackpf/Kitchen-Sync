package com.jackpf.kitchensync;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Arrays;

/**
 * Created by jackfarrelly on 24/01/2016.
 */
public class Executor {
    private String file;

    public Executor(String file) throws Exception {
        this.file = file;

        Process p = Runtime.getRuntime().exec("which " + file);
        p.waitFor();
        BufferedReader reader = new BufferedReader(new InputStreamReader(p.getInputStream()));
        if (reader.readLine() == null) {
            throw new RuntimeException("Could not find ffmpeg binary, is it installed?");
        }
    }

    public void run(String[] args) throws IOException, InterruptedException {
        String[] cmdArgs = new String[args.length + 1];
        cmdArgs[0] = file;
        for (int i = 0; i < args.length; i++) {
            cmdArgs[i + 1] = args[i];
        }

        System.out.println("Executing: " + Arrays.toString(cmdArgs));

        Process p = Runtime.getRuntime().exec(cmdArgs);
        int r = p.waitFor();

        BufferedReader stdReader = new BufferedReader(new InputStreamReader(p.getInputStream()));
        String stdLine;
        StringBuffer stdSb = new StringBuffer();
        while ((stdLine = stdReader.readLine())!= null) {
            System.out.println("Output (std): " + stdLine);
            stdSb.append(stdLine);
        }

        BufferedReader errReader = new BufferedReader(new InputStreamReader(p.getErrorStream()));
        String errLine;
        StringBuffer errSb = new StringBuffer();
        while ((errLine = errReader.readLine())!= null) {
            System.out.println("Output (err): " + errLine);
            errSb.append(errLine);
        }

        if (r != 0) {
            throw new IOException("Process returned non-zero exit code(" + r + "): " + errSb.toString());
        }
    }
}
