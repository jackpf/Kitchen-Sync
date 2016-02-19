package com.jackpf.kitchensync;

import javafx.application.Platform;
import javafx.scene.control.Alert;

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
}
