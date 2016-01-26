package com.jackpf.kitchensync;

import TrackAnalyzer.TrackAnalyzer;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.cell.TextFieldTableCell;
import javafx.stage.FileChooser;
import javafx.stage.Stage;

import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * Created by jackfarrelly on 25/01/2016.
 */
public class Controller {
    private Stage stage;

    private Executor ffmpeg, rubberband;

    @FXML
    private TableView<Info> tracks;

    @FXML
    private TableColumn bpm;

    public void initialise(Stage stage) {
        this.stage = stage;
        bpm.setCellFactory(TextFieldTableCell.<Info>forTableColumn());

        ffmpeg = new Executor("ffmpeg");
        rubberband = new Executor("rubberband");
    }

    protected void addTrack(String filename) throws Exception {
        TrackAnalyzer analyzer = new TrackAnalyzer(new String[]{filename, "-w", "-o", "/tmp/trackanalyzer.txt"});
        TrackAnalyzer.Info info = analyzer.analyzeTrack(filename, false);

        ObservableList<Info> data = tracks.getItems();
        Info trackInfo = new Info(info.filename, Integer.toString(info.bpm));
        data.add(trackInfo);
    }

    @FXML
    protected void addFiles(ActionEvent event) {
        FileChooser fileChooser = new FileChooser();
        FileChooser.ExtensionFilter extFilter = new FileChooser.ExtensionFilter("Audio files", new ArrayList<String>(Arrays.asList("*.mp3", "*.wav")));
        fileChooser.getExtensionFilters().add(extFilter);
        List<File> files = fileChooser.showOpenMultipleDialog(stage);
        for (File file : files) {
            try {
                addTrack(file.getAbsolutePath());
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    @FXML
    protected void run(ActionEvent event) {
        //ffmpeg.run(new String[]{"-i", info.filename, "/tmp/tmp.wav"});
        //rubberband.run(new String[]{"--tempo", Float.toString(targetBpm / info.bpm), "/tmp/tmp.wav", "/Users/jackfarrelly/Downloads/out.wav"});
    }
}
