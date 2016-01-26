package com.jackpf.kitchensync;

import TrackAnalyzer.TrackAnalyzer;
import javafx.collections.ObservableList;
import javafx.concurrent.Service;
import javafx.concurrent.Task;
import javafx.concurrent.WorkerStateEvent;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.fxml.FXML;
import javafx.scene.control.ProgressIndicator;
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

    @FXML
    private ProgressIndicator progressIndicator;

    public void initialise(Stage stage) {
        this.stage = stage;
        bpm.setCellFactory(TextFieldTableCell.<Info>forTableColumn());

        ffmpeg = new Executor("ffmpeg");
        rubberband = new Executor("rubberband");
    }

    public class ServiceExample extends Service<TrackAnalyzer.Info> {
        private final Info trackInfo;

        public ServiceExample(Info trackInfo) {
            this.trackInfo = trackInfo;
        }

        @Override
        protected Task<TrackAnalyzer.Info> createTask() {
            return new Task<TrackAnalyzer.Info>() {
                @Override
                protected TrackAnalyzer.Info call() throws Exception {
                    TrackAnalyzer analyzer = new TrackAnalyzer(new String[]{trackInfo.getFilename(), "-w", "-o", "/tmp/trackanalyzer.txt"});
                    TrackAnalyzer.Info info = analyzer.analyzeTrack(trackInfo.getFilename(), false);
                    return info;
                }
            };
        }
    }

    protected void addTrack(File file) {
        final ObservableList<Info> data = tracks.getItems();

        Info trackInfo = new Info(file.getAbsolutePath(), "-");
        data.add(trackInfo);

        final ServiceExample serviceExample = new ServiceExample(trackInfo);

        //Here you tell your progress indicator is visible only when the service is runing
        progressIndicator.visibleProperty().bind(serviceExample.runningProperty());
        serviceExample.setOnSucceeded(new EventHandler<WorkerStateEvent>() {
            public void handle(WorkerStateEvent workerStateEvent) {
                TrackAnalyzer.Info info = serviceExample.getValue();
                for (Info track : data) {
                    if (track.getFilename().equals(info.filename)) {
                        track.setBpm(Integer.toString(info.bpm));
                        continue;
                    }
                }
            }
        });

        /*serviceExample.setOnFailed(new EventHandler<WorkerStateEvent>() {
            public void handle(WorkerStateEvent workerStateEvent) {
                //DO stuff on failed
            }
        });*/
        serviceExample.start(); //here you start your service
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
        //ffmpeg.run(new String[]{"-i", info.filename, "/tmp/tmp.wav"});
        //rubberband.run(new String[]{"--tempo", Float.toString(targetBpm / info.bpm), "/tmp/tmp.wav", "/Users/jackfarrelly/Downloads/out.wav"});
    }
}
