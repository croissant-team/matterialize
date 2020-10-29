package uk.ac.ic.benchmatte

import uk.ac.ic.benchmatte.util.Benchmark

fun main() {
    nu.pattern.OpenCV.loadLocally()

    val benchmark = Benchmark("images/masks", "images/bg", "images/fg")

    benchmark.setup()
    benchmark.export()

    val results = benchmark.run()

    for (i in results.indices) {
        val imgResults = results[i]
        println("----------------------------------------------------------------")
        imgResults.forEach { (mode, matrix, time) ->
            val truePositives = matrix[1][1]
            val falseNegatives = matrix[0][1]
            val falsePositives = matrix[1][0]
            val trueNegatives = matrix[0][0]
            val precision = truePositives.toDouble() / (truePositives + falsePositives).toDouble()
            val recall = truePositives.toDouble() / (truePositives + falseNegatives).toDouble()
            val f1 = (2.0 * precision * recall) / (precision + recall)
            val accuracy = (truePositives + trueNegatives).toDouble() /
                (truePositives + trueNegatives + falsePositives + falseNegatives).toDouble()
            println(
                """
                Confusion Matrix ($mode took ${time}ms for image $i)
                  precision:  $precision
                  recall:     $recall
                  f1:         $f1
                  accuracy:   $accuracy

                                [Predicted]
                                fg   |   bg
                            +--------+--------+
                         fg | ${pad(truePositives)} | ${pad(falseNegatives)} |
                [Actual] ---+--------+--------+
                         bg | ${pad(falsePositives)} | ${pad(trueNegatives)} |
                            +--------+--------+
                            

            """.trimIndent()
            )
        }
    }
}

fun pad(count: Int, len: Int = 6): String {
    return count.toString().padStart(len, ' ')
}
