package uk.ac.ic.matterialize.gui

import javafx.geometry.Pos
import javafx.scene.control.Label
import javafx.scene.control.ToggleGroup
import javafx.stage.FileChooser
import tornadofx.View
import tornadofx.action
import tornadofx.borderpane
import tornadofx.button
import tornadofx.chooseFile
import tornadofx.hbox
import tornadofx.imageview
import tornadofx.label
import tornadofx.radiobutton
import tornadofx.stackpane
import tornadofx.useMaxWidth
import tornadofx.vbox
import uk.ac.ic.matterialize.matting.MatterMode

class MatterializeView : View("Matterialize") {

    private val webcamViewController: MatterializeController by inject()
    private val matterGroup = ToggleGroup()

    override val root = stackpane {
        setPrefSize(2 * webcamViewController.getImageWidth(), 150.0)

        val label = Label("No image selected")

        vbox {
            hbox {
                imageview {
                    webcamViewController.setInputImage(this)
                }
                imageview {
                    webcamViewController.setOutputImage(this)
                }
                webcamViewController.initialiseWebCamThread()
            }

            borderpane {
                top = label {
                    useMaxWidth = true
                }

                center = button("Select a background image") {
                    action {
                        val file = chooseFile(
                            "Select a background image",
                            arrayOf(
                                FileChooser.ExtensionFilter("image", "*.jpg", "*.jpeg", "*.png", "*.gif")
                            )
                        )

                        file.firstOrNull()?.let {
                            webcamViewController.applyNewBackground(it.absolutePath)
                            label.text = it.name
                        }
                    }
                }
            }

            borderpane {
                top = label {
                    useMaxWidth = true
                }

                bottom = label {
                    useMaxWidth = true
                }

                center = label
            }

            borderpane {
                top = Label("Select a matter")

                center = vbox {
                    alignment = Pos.CENTER

                    val buttons = mapOf(
                        "KMeans" to MatterMode.KMeans,
                        "OpenCV" to MatterMode.OpenCV,
                        "Background Negation" to MatterMode.BackgroundNegation,
                        "Face Detection" to MatterMode.FaceDetection
                    )

                    buttons.forEach { (text, name) ->
                        radiobutton(text, matterGroup) {
                            alignment = Pos.BOTTOM_LEFT
                            action {
                                if (isSelected) webcamViewController.setMatter(name)
                            }
                        }
                    }
                }

                bottom = label {
                }
            }
        }
    }
}
