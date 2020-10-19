package uk.ac.ic.matterialize.gui

import tornadofx.App
import kotlin.system.exitProcess

class MatterializeApp : App(MatterializeView::class) {

    private val matterializeController: MatterializeController by inject()

    override fun stop() {
        matterializeController.joinThreads()
        exitProcess(0)
    }
}
