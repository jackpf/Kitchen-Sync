package com.jackpf.kitchensync;

import TrackAnalyzer.TrackAnalyzer;
import javafx.collections.ObservableList;
import javafx.concurrent.Service;
import javafx.concurrent.Task;
import javafx.concurrent.WorkerStateEvent;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.fxml.FXML;
import javafx.scene.control.*;
import javafx.scene.control.cell.TextFieldTableCell;
import javafx.stage.FileChooser;
import javafx.stage.Stage;
import sun.awt.Mutex;

import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Optional;

/**
 * Created by jackfarrelly on 25/01/2016.
 */
public class Controller {
    private Stage stage;

    private Executor ffmpeg, rubberband;

    private static final Mutex analyzerMutex = new Mutex();

    @FXML
    private TableView<Info> tracks;

    @FXML
    private TableColumn bpm;

    @FXML
    private ProgressIndicator progressIndicator;

    @FXML
    private Button addButton;

    @FXML
    private Button runButton;

    public void initialise(Stage stage) {
        this.stage = stage;
        bpm.setCellFactory(TextFieldTableCell.<Info>forTableColumn());
        bpm.setOnEditCommit(new EventHandler<TableColumn.CellEditEvent<Info, String>>() {
            public void handle(TableColumn.CellEditEvent<Info, String> event) {
                event.getRowValue().setBpm(event.getNewValue());
                enableRunButtonIfReady();
            }
        });

        ffmpeg = new Executor("ffmpeg");
        rubberband = new Executor("rubberband");
    }

    public class AnalyzerService extends Service<TrackAnalyzer.Info> {
        private final Info trackInfo;

        public AnalyzerService(Info trackInfo) {
            this.trackInfo = trackInfo;
        }

        @Override
        protected Task<TrackAnalyzer.Info> createTask() {
            return new Task<TrackAnalyzer.Info>() {
                @Override
                protected TrackAnalyzer.Info call() throws Exception {
                    analyzerMutex.lock();
                    TrackAnalyzer analyzer = new TrackAnalyzer(new String[]{trackInfo.getFilename(), "-w", "-o", "/tmp/trackanalyzer.txt"});
                    TrackAnalyzer.Info info = analyzer.analyzeTrack(trackInfo.getFilename(), false);
                    analyzerMutex.unlock();
                    return info;
                }
            };
        }
    }

    protected void enableRunButtonIfReady() {
        ObservableList<Info> data = tracks.getItems();
        boolean isRunnable = true;

        try {
            for (Info track : data) {
                Integer.parseInt(track.getBpm());
            }
        } catch (NumberFormatException e) {
            isRunnable = false;
        }

        runButton.setDisable(!isRunnable);
    }

    protected void addTrack(File file) {
        final ObservableList<Info> data = tracks.getItems();

        Info trackInfo = new Info(file.getAbsolutePath(), file.getName(), Info.NO_BPM);
        data.add(trackInfo);

        final AnalyzerService serviceExample = new AnalyzerService(trackInfo);

        progressIndicator.visibleProperty().bind(serviceExample.runningProperty());
        serviceExample.setOnSucceeded(new EventHandler<WorkerStateEvent>() {
            public void handle(WorkerStateEvent workerStateEvent) {
                TrackAnalyzer.Info info = serviceExample.getValue();

                for (Info track : data) {
                    if (track.getFilename().equals(info.filename)) {
                        track.setBpm(Integer.toString(info.bpm));
                    }
                }

                enableRunButtonIfReady();
            }
        });

        serviceExample.start();
    }

    @FXML
    protected void addFiles(ActionEvent event) {
        FileChooser fileChooser = new FileChooser();
        FileChooser.ExtensionFilter extFilter = new FileChooser.ExtensionFilter("Audio files", new ArrayList<String>(Arrays.asList("*.mp3", "*.wav")));
        fileChooser.getExtensionFilters().add(extFilter);
        List<File> files = fileChooser.showOpenMultipleDialog(stage);

        for (File file : files) {
            addTrack(file);
        }
    }

    @FXML
    protected void run(ActionEvent event) {
        TextInputDialog dialog = new TextInputDialog();
        dialog.setTitle("Target BPM");
        dialog.setHeaderText("Set target BPM");
        dialog.setContentText("Enter target BPM:");

        Optional<String> result = dialog.showAndWait();
        if (result.isPresent()){
            System.out.println("Target BPM: " + result.get());
        }

        //ffmpeg.run(new String[]{"-i", info.filename, "/tmp/tmp.wav"});
        //rubberband.run(new String[]{"--tempo", Float.toString(targetBpm / info.bpm), "/tmp/tmp.wav", "/Users/jackfarrelly/Downloads/out.wav"});
    }
}
