package com.jackpf.kitchensync.executor;

import com.jackpf.kitchensync.Executor;
import org.apache.commons.lang3.tuple.Pair;

import java.io.IOException;
import java.util.List;

/**
 * Created by jackfarrelly on 19/02/2016.
 */
public class FFMPEGExecutor extends Executor {
    public FFMPEGExecutor() throws Exception {
        //super(URLDecoder.decode(ClassLoader.getSystemClassLoader().getResource("ffmpeg").getPath(), "UTF-8"), new String[]{"-y"});
        super("ffmpeg", new String[]{"-y"});
    }

    @Override
    public Pair<List<String>, List<String>> run(String[] args) throws IOException, InterruptedException {
        Pair<List<String>, List<String>> lines = super.run(args);

        // Just do an extra check for any error strings in the output
        // as ffmpeg returns 0 error code even if it hasn't successfully decoded

        for (String line : lines.getRight()) {
            if (line.contains("Error")) {
                throw new IOException("FFMPEG error: " + line);
            }
        }

        return lines;
    }
}
