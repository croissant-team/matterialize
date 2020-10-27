package uk.ac.ic.matterialize.matting.backgroundcut

import org.opencv.core.Core
import org.opencv.core.Mat
import org.opencv.ml.EM

object GMMGlobalColorModel {
    // TODO Check if this is incorrect, but I'm pretty sure it does exactly equation (2)
    fun globalProbs(trainedGMM: EM, image: FlatImage): Probs {
        val distribProbs = Mat()
        val samples = image.toSamples()
        trainedGMM.predict(samples, distribProbs)
        val ROW_SUM = 1
        val weights = Mat()
        Core.transpose(trainedGMM.weights, weights)
        val probs = Mat()

        Core.gemm(distribProbs, weights, 1.0, Mat(), 0.0, probs, 0)
        Core.reduce(probs, probs, ROW_SUM, Core.REDUCE_SUM)

        return Probs(probs)
    }
}

