package com.jackpf.kitchensync;

import com.jackpf.kitchensync.CInterface.CInterface;
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

        ffmpeg = new Executor("/usr/local/bin/ffmpeg");
    }

    @Override
    protected Task<Info> createTask() {
        return new Task<Info>() {
            @Override
            protected Info call() throws Exception {
                // Needs to be smart mutex!
                mutex.lock();

                ffmpeg.run(new String[]{"-y", "-i", trackInfo.getFile().getAbsolutePath(), trackInfo.getTmpFile().getAbsolutePath()});

                if (!trackInfo.getTmpFile().exists()) {
                    throw new IOException("File " + trackInfo.getTmpFile().getAbsolutePath() + " does not exist");
                }

                cInterface.setBpm(
                    trackInfo.getTmpFile().getAbsolutePath(),
                    trackInfo.getTmpFile2().getAbsolutePath(),
                    Float.parseFloat(trackInfo.getBpm()),
                    targetBpm
                );

                trackInfo.getTmpFile().delete();

                if (!trackInfo.getTmpFile2().exists()) {
                    throw new IOException("File " + trackInfo.getTmpFile2().getAbsolutePath() + " does not exist");
                }

                TagWriter.Tags tags = new TagWriter(trackInfo).getTags();

                ffmpeg.run(new String[]{
                    "-y",
                    "-i",
                    trackInfo.getTmpFile2().getAbsolutePath(),
                    "-ab", "320k",
                    "-ac", "2",
                    "-metadata", "title=" + tags.name + "",
                    "-metadata", "artist=" + tags.artist + "",
                    outputDir.getParent() + "/" + trackInfo.getFile().getName()
                });

                trackInfo.getTmpFile2().delete();

                mutex.unlock();

                return trackInfo;
            }
        };
    }
}
