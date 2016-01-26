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
        StringBuilder sb = new StringBuilder(file);
        for (String arg : args) {
            sb.append(" \"" + arg.replaceAll("\"", "\\\"") + "\"");
        }

        Process p = Runtime.getRuntime().exec(sb.toString());
        p.waitFor();

        BufferedReader reader = new BufferedReader(new InputStreamReader(p.getInputStream()));

        String line;
        while ((line = reader.readLine())!= null) {
            System.out.println(line);
        }
    }
}
