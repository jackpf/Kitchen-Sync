package com.jackpf.kitchensync;

import javafx.concurrent.Service;
import javafx.concurrent.Task;
import sun.awt.Mutex;

import java.io.File;

/**
 * Created by jackfarrelly on 26/01/2016.
 */
public class ProcessorService extends Service<Info> {
    private static final Mutex mutex = new Mutex();

    private final String TMP_FILE = "/tmp/tmp.wav", TMP_FILE2 = "/tmp/tmp2.wav";

    private Executor ffmpeg, rubberband;

    private final Info trackInfo;

    private final int targetBpm;

    private final File outputDir;

    public ProcessorService(Info trackInfo, int targetBpm, File outputDir) {
        this.trackInfo = trackInfo;
        this.targetBpm = targetBpm;
        this.outputDir = outputDir;

        ffmpeg = new Executor("/usr/local/bin/ffmpeg");
        rubberband = new Executor("/usr/local/bin/rubberband");
    }

    @Override
    protected Task<Info> createTask() {
        return new Task<Info>() {
            @Override
            protected Info call() throws Exception {
                mutex.lock();
                ffmpeg.run(new String[]{"-y", "-i", trackInfo.getFilename(), TMP_FILE});
                rubberband.run(new String[]{"--tempo", Float.toString(targetBpm / Float.parseFloat(trackInfo.getBpm())), TMP_FILE, TMP_FILE2});
                new File(TMP_FILE).delete();
                ffmpeg.run(new String[]{"-y", "-i", TMP_FILE2, outputDir.getParent() + "/" + trackInfo.getFile().getName()});
                new File(TMP_FILE2).delete();
                mutex.unlock();
                return trackInfo;
            }
        };
    }
}
