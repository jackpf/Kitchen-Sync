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
    private Button saveButton;

    @FXML
    private Button burnButton;

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

        stage.getScene().setOnDragOver((DragEvent event) -> {
            Dragboard db = event.getDragboard();
            if (db.hasFiles()) {
                event.acceptTransferModes(TransferMode.COPY);
            } else {
                event.consume();
            }
        });
        stage.getScene().setOnDragDropped((DragEvent event) -> {
            Dragboard db = event.getDragboard();
            boolean success = false;
            if (db.hasFiles()) {
                for (File file : db.getFiles()) {
                    if (file.getName().toLowerCase().endsWith(".mp3") || file.getName().toLowerCase().endsWith(".wav")) {
                        try {
                            addTrack(file);
                            success = true;
                        }  catch (Exception e) {
                            Helpers.handleError(e);
                        }
                    }
                }
            }
            event.setDropCompleted(success);
            event.consume();
        });

        bpm.setCellFactory(TextFieldTableCell.<Info>forTableColumn());
        bpm.setOnEditCommit(new EventHandler<TableColumn.CellEditEvent<Info, String>>() {
            public void handle(TableColumn.CellEditEvent<Info, String> event) {
                try {
                    event.getRowValue().setBpm(String.format("%.1f", Float.parseFloat(event.getNewValue())));
                } catch (NumberFormatException e) {
                    event.getRowValue().setBpm(event.getNewValue());
                }
                enableButtonsIfReady();
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
                        Helpers.handleError(e);
                    }
                }
            });
            return row;
        });
        tracks.getSelectionModel().getSelectedIndices().addListener((ListChangeListener.Change<? extends Integer> change) -> {
            if (change.getList().size() > 0) {
                removeButton.setDisable(false);
            } else {
                removeButton.setDisable(true);
            }
        });

        stage.show();
    }

    protected void enableButtonsIfReady() {
        ObservableList<Info> data = tracks.getItems();
        boolean isRunnable = true;

        try {
            for (Info track : data) {
                Float.parseFloat(track.getBpm());
            }
        } catch (NumberFormatException e) {
            isRunnable = false;
        }

        saveButton.setDisable(!isRunnable);
        burnButton.setDisable(!isRunnable);
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

    protected void addTrack(File file) throws Exception {
        final ObservableList<Info> data = tracks.getItems();

        final Info trackInfo = new Info(file, Info.NO_BPM);

        for (Info track : data) {
            if (track.getFile().getAbsolutePath().equals(trackInfo.getFile().getAbsolutePath())) {
                return;
            }
        }

        if (!file.getAbsolutePath().matches("\\A\\p{ASCII}*\\z")) {
            throw new RuntimeException(file.getName() + " contains non ascii characters. This version of Kitchen Sync can not process this file, rename the file to add it.");
        }

        data.add(trackInfo);

        final AnalyserService analyzer = new AnalyserService(trackInfo);
        trackInfo.setService(analyzer);

        analyzer.setOnSucceeded((WorkerStateEvent workerStateEvent) -> {
            float bpm = analyzer.getValue();
            trackInfo.setBpm(String.format("%.1f", (float) Math.round(bpm)));

            enableButtonsIfReady();
            updateOutliers();
        });
        analyzer.setOnFailed((WorkerStateEvent workerStateEvent) -> {
            Helpers.handleError(workerStateEvent.getSource().getException());
        });

        analyzer.start();

        progressIndicator.visibleProperty().bind(analyzer.runningProperty());
    }

    @FXML
    protected void addFiles(ActionEvent event) {
        FileChooser fileChooser = new FileChooser();
        FileChooser.ExtensionFilter extFilter = new FileChooser.ExtensionFilter("Audio files", new ArrayList<>(Arrays.asList("*.mp3", "*.wav")));
        fileChooser.getExtensionFilters().add(extFilter);
        List<File> files = fileChooser.showOpenMultipleDialog(stage);

        for (File file : files) {
            try {
                addTrack(file);
            } catch (Exception e) {
                Helpers.handleError(e);
            }
        }
    }

    @FXML
    protected void save(ActionEvent event) {
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

                        processor.setOnSucceeded((WorkerStateEvent workerStateEvent) -> {
                            Info track = processor.getValue();
                            data.remove(track);
                        });
                        processor.setOnFailed((WorkerStateEvent workerStateEvent) -> {
                            Helpers.handleError(workerStateEvent.getSource().getException());
                        });

                        processor.start();

                        progressIndicator.visibleProperty().bind(processor.runningProperty());
                    } catch (Exception e) {
                        Helpers.handleError(e);
                    }
                }
            }
        }
    }

    @FXML
    protected void burn(ActionEvent event) {
        Helpers.alert("Oops", "Not yet implemented");
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
}
