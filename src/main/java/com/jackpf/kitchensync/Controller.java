package com.jackpf.kitchensync;

import javafx.collections.ListChangeListener;
import javafx.collections.ObservableList;
import javafx.concurrent.WorkerStateEvent;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.fxml.FXML;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.control.cell.TextFieldTableCell;
import javafx.scene.input.DragEvent;
import javafx.scene.input.Dragboard;
import javafx.scene.input.TransferMode;
import javafx.stage.FileChooser;
import javafx.stage.Stage;

import java.io.File;
import java.io.UnsupportedEncodingException;
import java.net.URLDecoder;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Optional;

/**
 * Created by jackfarrelly on 25/01/2016.
 */
public class Controller {
    private Stage stage;

    private Parent root;

    @FXML
    private TableView<Info> tracks;

    @FXML
    private TableColumn filename;

    @FXML
    private TableColumn bpm;

    @FXML
    private ProgressIndicator progressIndicator;

    @FXML
    private Button addButton;

    @FXML
    private Button runButton;

    @FXML
    private Button removeButton;

    private Executor spek;

    public Controller() {
        try {
            spek = new Executor(
                "open",
                new String[]{
                    URLDecoder.decode(ClassLoader.getSystemClassLoader().getResource("Spek.app").getPath(), "UTF-8"),
                    "--args"
                }
            );
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }
    }

    public void initialise(Parent root, Stage stage) {
        this.stage = stage;

        stage.setTitle("Kitchen Sync");
        stage.setScene(new Scene(root, 300, 275));
        stage.setResizable(false);

        stage.getScene().setOnDragOver(new EventHandler<DragEvent>() {
            public void handle(DragEvent event) {
                Dragboard db = event.getDragboard();
                if (db.hasFiles()) {
                    event.acceptTransferModes(TransferMode.COPY);
                } else {
                    event.consume();
                }
            }
        });
        stage.getScene().setOnDragDropped(new EventHandler<DragEvent>() {
            public void handle(DragEvent event) {
                Dragboard db = event.getDragboard();
                boolean success = false;
                if (db.hasFiles()) {
                    for (File file : db.getFiles()) {
                        if (file.getName().toLowerCase().endsWith(".mp3") || file.getName().toLowerCase().endsWith(".wav")) {
                            addTrack(file);
                            success = true;
                        }
                    }
                }
                event.setDropCompleted(success);
                event.consume();
            }
        });

        bpm.setCellFactory(TextFieldTableCell.<Info>forTableColumn());
        bpm.setOnEditCommit(new EventHandler<TableColumn.CellEditEvent<Info, String>>() {
            public void handle(TableColumn.CellEditEvent<Info, String> event) {
                try {
                    event.getRowValue().setBpm(String.format("%.1f", Float.parseFloat(event.getNewValue())));
                } catch (NumberFormatException e) {
                    event.getRowValue().setBpm(event.getNewValue());
                }
                enableRunButtonIfReady();
            }
        });
        tracks.setRowFactory(tv -> {
            TableRow<Info> row = new TableRow<>();
            row.setOnMouseClicked(event -> {
                if (event.getClickCount() == 2 && (!row.isEmpty()) ) {
                    Info info = row.getItem();
                    try {
                        spek.run(new String[]{info.getFile().getPath()});
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
            });
            return row;
        });
        tracks.getSelectionModel().getSelectedIndices().addListener(new ListChangeListener<Integer>() {
            public void onChanged(Change<? extends Integer> change) {
                if (change.getList().size() > 0) {
                    removeButton.setDisable(false);
                } else {
                    removeButton.setDisable(true);
                }
            }

        });

        stage.show();
    }

    protected void enableRunButtonIfReady() {
        ObservableList<Info> data = tracks.getItems();
        boolean isRunnable = true;

        try {
            for (Info track : data) {
                Float.parseFloat(track.getBpm());
            }
        } catch (NumberFormatException e) {
            isRunnable = false;
        }

        runButton.setDisable(!isRunnable);
    }

    protected void updateOutliers() {
        ObservableList<Info> data = tracks.getItems();
        final String ATN_STR = " - ?";

        float totalBpm = 0;
        int count = 0;

        for (Info track : data) {
            try {
                totalBpm += Float.parseFloat(track.getBpm());
                count++;
            } catch (NumberFormatException e) {
                continue;
            }
        }

        float avg = totalBpm / count;

        for (Info track : data) {
            try {
                float bpm = Float.parseFloat(track.getBpm().replace(ATN_STR, ""));

                if ((bpm > avg + avg * 0.1 || bpm < avg - avg * 0.1) && !track.getBpm().contains(ATN_STR)) {
                    track.setBpm(track.getBpm() + ATN_STR);
                } else if (track.getBpm().contains(ATN_STR) && (bpm <= avg + avg * 0.1 && bpm >= avg - avg * 0.1)) {
                    track.setBpm(track.getBpm().replace(ATN_STR, ""));
                }
            } catch (NumberFormatException e) {
                continue;
            }
        }
    }

    protected void addTrack(File file) {
        final ObservableList<Info> data = tracks.getItems();

        final Info trackInfo = new Info(file, Info.NO_BPM);

        // Needs to check filename instead
        for (Info track : data) {
            if (track.getFilename().equals(trackInfo.getFilename())) {
                return;
            }
        }

        if (!file.getAbsolutePath().matches("\\A\\p{ASCII}*\\z")) {
            alert("Error", file.getName() + " contains non ascii characters. Kitchen Sync can currently not process this file, rename the file to add it.");
            return;
        }

        data.add(trackInfo);

        try {
            final AnalyserService analyzer = new AnalyserService(trackInfo);
            trackInfo.setService(analyzer);

            progressIndicator.visibleProperty().bind(analyzer.runningProperty());
            analyzer.setOnSucceeded(new EventHandler<WorkerStateEvent>() {
                public void handle(WorkerStateEvent workerStateEvent) {
                    float bpm = analyzer.getValue();
                    trackInfo.setBpm(String.format("%.1f", (float) Math.round(bpm)));

                    enableRunButtonIfReady();
                    updateOutliers();
                }
            });
            analyzer.setOnFailed(new EventHandler<WorkerStateEvent>() {
                public void handle(WorkerStateEvent workerStateEvent) {
                    workerStateEvent.getSource().getException().printStackTrace();
                    alert("Error", "Unable to analyse " + trackInfo.getFile().getName() + ": " + workerStateEvent.getSource().getException().getMessage());
                }
            });

            analyzer.start();
        } catch (Exception e) {
            e.printStackTrace();
            alert("Error", "Unable to analyse " + file.getName() + ": " + e.getMessage());
        }
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
                for (final Info info : data) {
                    try {
                        final ProcessorService processor = new ProcessorService(info, Integer.parseInt(result.get()), dir);

                        progressIndicator.visibleProperty().bind(processor.runningProperty());
                        processor.setOnSucceeded(new EventHandler<WorkerStateEvent>() {
                            public void handle(WorkerStateEvent workerStateEvent) {
                                Info track = processor.getValue();
                                data.remove(track);
                            }
                        });
                        processor.setOnFailed(new EventHandler<WorkerStateEvent>() {
                            public void handle(WorkerStateEvent workerStateEvent) {
                                workerStateEvent.getSource().getException().printStackTrace();
                                alert("Error", "Unable to process " + info.getFile().getName() + ": " + workerStateEvent.getSource().getException().getMessage());
                            }
                        });
                        processor.start();
                    } catch (Exception e) {
                        e.printStackTrace();
                        alert("Error", "Unable to process " + info.getFile().getName() + ": " + e.getMessage());
                    }
                }
            }
        }
    }

    @FXML
    protected void removeFiles(ActionEvent event) {
        tracks.getItems().removeAll(tracks.getSelectionModel().getSelectedItems());

        for (Info track : tracks.getSelectionModel().getSelectedItems()) {
            if (track.getService().isRunning()) {
                track.getService().cancel();
            }
        }
    }

    protected void alert(String title, String message) {
        Alert alert = new Alert(Alert.AlertType.INFORMATION);
        alert.setTitle(title);
        alert.setHeaderText(null);
        alert.setContentText(message);

        alert.showAndWait();
    }
}
