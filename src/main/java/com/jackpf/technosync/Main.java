package com.jackpf.technosync;

import TrackAnalyzer.TrackAnalyzer;
import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

/**
 * Created by jackfarrelly on 23/01/2016.
 */
public class Main extends Application {
    public static void main(String[] args) {
        launch(args);
    }

    @Override
    public void start(Stage stage) throws Exception {
        try {
            String f = "/Users/jackfarrelly/Downloads/secluded.wav",
                f2 = "/Users/jackfarrelly/Downloads/secluded-new.wav";

            float targetBpm = 130;

            Executor ffmpeg = new Executor("ffmpeg"),
                rubberband = new Executor("rubberband");

            TrackAnalyzer analyzer = new TrackAnalyzer(new String[]{f, "-w", "-o", "/Users/jackfarrelly/results.txt"});
            TrackAnalyzer.Info info = analyzer.analyzeTrack(f, false);

            System.out.println(info.filename + ": " + info.bpm + "bpm");

            Parent root = FXMLLoader.load(getClass().getResource("/layout/main.fxml"));
            stage.setTitle("Techno Sync");
            stage.setScene(new Scene(root, 300, 275));
            stage.show();

            //ffmpeg.run(new String[]{"-i", info.filename, "/tmp/tmp.wav"});
            //rubberband.run(new String[]{"--tempo", Float.toString(targetBpm / info.bpm), "/tmp/tmp.wav", "/Users/jackfarrelly/Downloads/out.wav"});
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
