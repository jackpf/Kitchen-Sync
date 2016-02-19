package com.jackpf.kitchensync;

import javafx.application.Platform;
import javafx.scene.control.Alert;
import javafx.scene.control.TableView;
import javafx.scene.control.TextInputDialog;
import javafx.stage.FileChooser;
import javafx.stage.Stage;

import java.io.File;
import java.util.Optional;

/**
 * Created by jackfarrelly on 19/02/2016.
 */
public class Helpers {
    public static void alert(String title, String message) {
        Platform.runLater(() -> {
            Alert alert = new Alert(Alert.AlertType.INFORMATION);
            alert.setTitle(title);
            alert.setHeaderText(null);
            alert.setContentText(message);

            alert.showAndWait();
        });
    }

    public static void handleError(Throwable e) {
        e.printStackTrace();

        alert("Error", e.getMessage());
    }

    public static void redraw(TableView<?> table) {
        table.getColumns().get(0).setVisible(false);
        table.getColumns().get(0).setVisible(true);
    }

    public static Optional<String> prompt(String title, String header, String message) {
        TextInputDialog dialog = new TextInputDialog();

        dialog.setTitle(title);
        dialog.setHeaderText(header);
        dialog.setContentText(message);

        return dialog.showAndWait();
    }

    public static File promptDir(Stage stage) {
        FileChooser fileChooser = new FileChooser();
        return fileChooser.showSaveDialog(stage);
    }
}
