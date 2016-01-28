package com.jackpf.kitchensync;

import javafx.collections.ObservableList;
import javafx.concurrent.WorkerStateEvent;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.fxml.FXML;
import javafx.scene.control.*;
import javafx.scene.control.cell.TextFieldTableCell;
import javafx.stage.FileChooser;
import javafx.stage.Stage;

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

        final Info trackInfo = new Info(file, Info.NO_BPM);

        if (data.contains(trackInfo)) {
            return;
        } else {
            data.add(trackInfo);
        }

        final AnalyzerService analyzer = new AnalyzerService(trackInfo);

        progressIndicator.visibleProperty().bind(analyzer.runningProperty());
        analyzer.setOnSucceeded(new EventHandler<WorkerStateEvent>() {
            public void handle(WorkerStateEvent workerStateEvent) {
                String bpm = analyzer.getValue();
                trackInfo.setBpm(bpm);

                enableRunButtonIfReady();
            }
        });

        analyzer.start();
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
            FileChooser fileChooser = new FileChooser();
            File dir = fileChooser.showSaveDialog(stage);

            if (dir != null) {
                final ObservableList<Info> data = tracks.getItems();
                for (Info info : data) {
                    final ProcessorService processor = new ProcessorService(info, Integer.parseInt(result.get()), dir);

                    progressIndicator.visibleProperty().bind(processor.runningProperty());
                    processor.setOnSucceeded(new EventHandler<WorkerStateEvent>() {
                        public void handle(WorkerStateEvent workerStateEvent) {
                            Info track = processor.getValue();
                            data.remove(track);
                        }
                    });
                    processor.start();
                }
            }
        }
    }
}
