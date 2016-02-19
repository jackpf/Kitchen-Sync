package com.jackpf.kitchensync;

import com.jackpf.kitchensync.entity.Info;
import com.jackpf.kitchensync.entity.Tracks;
import com.jackpf.kitchensync.service.AnalyserService;
import com.jackpf.kitchensync.service.ProcessorService;
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
import java.util.List;
import java.util.Optional;

/**
 * Created by jackfarrelly on 25/01/2016.
 */
public class Controller {
    private Stage stage;

    private Parent root;

    @FXML
    private TableView<Info> tracksTable;

    @FXML
    private TableColumn filenameColumn;

    @FXML
    private TableColumn bpmColumn;

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

    private Tracks tracks;

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
        this.tracks = new Tracks(tracksTable.getItems());
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
                    try {
                        if (addTrack(file) != null) {
                            success = true;
                        }
                    }  catch (Exception e) {
                        Helpers.handleError(e);
                    }
                }
            }
            event.setDropCompleted(success);
            event.consume();
        });

        bpmColumn.setCellFactory(TextFieldTableCell.<Info>forTableColumn());
        bpmColumn.setOnEditCommit(new EventHandler<TableColumn.CellEditEvent<Info, String>>() {
            @Override
            public void handle(TableColumn.CellEditEvent<Info, String> event) {
                try {
                    event.getRowValue().setBpm(String.format("%.1f", Float.parseFloat(event.getNewValue())));
                } catch (NumberFormatException e) {
                    event.getRowValue().setBpm(event.getNewValue());
                }
                enableButtonsIfReady();
            }
        });
        tracksTable.setRowFactory(tv -> {
            TableRow<Info> row = new TableRow<Info>() {
                @Override
                protected void updateItem(Info info, boolean empty){
                    super.updateItem(info, empty);

                    if (info == null || empty || info.getQuality() == -1.0f) {
                        return;
                    }

                    System.out.println("QUALITY: "+info.getDisplayName()+": "+info.getQuality());

                    if (info.getQuality() <= 0.5) {
                        getStyleClass().add("quality-low");
                    } else if (info.getQuality() <= 0.75) {
                        getStyleClass().add("quality-medium");
                    } else {
                        getStyleClass().add("quality-high");
                    }
                }
            };

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
        tracksTable.getSelectionModel().getSelectedIndices().addListener((ListChangeListener.Change<? extends Integer> change) -> {
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

    protected Info addTrack(File file) throws Exception {
        Info trackInfo = tracks.addFile(file);

        if (trackInfo == null) {
            return null;
        }

        final AnalyserService analyzer = new AnalyserService(trackInfo);
        trackInfo.setService(analyzer);

        analyzer.setOnSucceeded((WorkerStateEvent workerStateEvent) -> {
            float bpm = analyzer.getValue();
            trackInfo.setBpm(String.format("%.1f", (float) Math.round(bpm)));

            enableButtonsIfReady();
            tracks.updateOutliers();

            Helpers.redraw(tracksTable); // Needed to update row styles
        });
        analyzer.setOnFailed((WorkerStateEvent workerStateEvent) -> {
            Helpers.handleError(workerStateEvent.getSource().getException());
        });

        analyzer.start();

        progressIndicator.visibleProperty().bind(analyzer.runningProperty());

        return trackInfo;
    }

    @FXML
    protected void addFiles(ActionEvent event) {
        FileChooser fileChooser = new FileChooser();
        fileChooser.getExtensionFilters().add(tracks.getExtensionFilter());
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
        Optional<String> result = Helpers.prompt("Target BPM", "Set target BPM", "Enter target BPM:");

        if (result.isPresent()){
            File dir = Helpers.promptDir(stage);

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
        tracks.getItems().removeAll(tracksTable.getSelectionModel().getSelectedItems());

        for (Info track : tracksTable.getSelectionModel().getSelectedItems()) {
            if (track.getService().isRunning()) {
                track.getService().cancel();
            }
        }
    }
}
