package com.jackpf.kitchensync;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

/**
 * Created by jackfarrelly on 24/01/2016.
 */
public class Executor {
    private String file;

    public Executor(String file) {
        this.file = file;
    }

    public void run(String[] args) throws IOException, InterruptedException {
        //System.out.println("Executing: " + sb.toString());

        String[] cmdArgs = new String[args.length + 1];
        cmdArgs[0] = file;
        for (int i = 0; i < args.length; i++) {
            cmdArgs[i + 1] = args[i];
        }

        Process p = Runtime.getRuntime().exec(cmdArgs);
        p.waitFor();

        System.out.println("Done");

        BufferedReader reader = new BufferedReader(new InputStreamReader(p.getInputStream()));
        String line;
        while ((line = reader.readLine())!= null) {
            System.out.println("Output: " + line);
        }

        BufferedReader reader2 = new BufferedReader(new InputStreamReader(p.getErrorStream()));
        String line2;
        while ((line2 = reader2.readLine())!= null) {
            System.out.println("Output: " + line2);
        }
    }
}
