package com.jackpf.kitchensync.service;

import com.jackpf.kitchensync.CInterface.CInterface;
import com.jackpf.kitchensync.Executor;
import com.jackpf.kitchensync.entity.Info;
import com.jackpf.kitchensync.TagWriter;
import com.jackpf.kitchensync.executor.FFMPEGExecutor;
import javafx.concurrent.Service;
import javafx.concurrent.Task;
import sun.awt.Mutex;

import java.io.File;
import java.io.IOException;

/**
 * Created by jackfarrelly on 26/01/2016.
 */
public class ProcessorService extends Service<Info> {
    private static final Mutex mutex = new Mutex();

    private CInterface cInterface = new CInterface();

    private Executor ffmpeg;

    private final Info trackInfo;

    private final float targetBpm;

    private final File outputDir;

    public ProcessorService(Info trackInfo, float targetBpm, File outputDir) throws Exception {
        this.trackInfo = trackInfo;
        this.targetBpm = targetBpm;
        this.outputDir = outputDir;

        ffmpeg = new FFMPEGExecutor();
    }

    @Override
    protected Task<Info> createTask() {
        return new Task<Info>() {
            @Override
            protected Info call() throws Exception {
                mutex.lock();

                try {
                    ffmpeg.run(new String[]{"-i", trackInfo.getFile().getAbsolutePath(), trackInfo.getTmpFile().getAbsolutePath()});

                    if (!trackInfo.getTmpFile().exists()) {
                        throw new IOException("File " + trackInfo.getTmpFile().getAbsolutePath() + " does not exist");
                    }

                    cInterface.setBpm(
                            trackInfo.getTmpFile().getAbsolutePath(),
                            trackInfo.getTmpFile2().getAbsolutePath(),
                            Float.parseFloat(trackInfo.getBpm()),
                            targetBpm
                    );

                    if (!trackInfo.getTmpFile2().exists()) {
                        throw new IOException("File " + trackInfo.getTmpFile2().getAbsolutePath() + " does not exist");
                    }

                    TagWriter.Tags tags = new TagWriter(trackInfo).getTags();

                    ffmpeg.run(new String[]{
                        "-i",
                        trackInfo.getTmpFile2().getAbsolutePath(),
                        "-ab", "320k",
                        "-ac", "2",
                        "-metadata", "title=" + tags.name + "",
                        "-metadata", "artist=" + tags.artist + "",
                        outputDir.getParent() + "/" + trackInfo.getFile().getName()
                    });

                    return trackInfo;
                } finally {
                    mutex.unlock();
                }
            }
        };
    }
}
