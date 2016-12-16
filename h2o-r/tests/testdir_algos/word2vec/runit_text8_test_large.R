setwd(normalizePath(dirname(R.utils::commandArgs(asValues=TRUE)$"f")))
source("../../../scripts/h2o-r-test-setup.R")




test.word2vec.sg_hs <- function() {
  text8.path = locate("bigdata/laptop/text8.gz")

  words <- h2o.importFile("/Users/mkurka/git/h2o/h2o-3/bigdata/laptop/text8.gz",
  destination_frame = "words", col.names = "word", col.types = "String")
  w2v <- h2o::h2o.word2vec(words, minWordFreq = 5, vecSize = 50, sentSampleRate = 0.001,
                           initLearningRate = 0.025, epochs = 1, windowSize = 4)
  synonyms <- h2o::h2o.findSynonyms(w2v, "dog", 20)
  known.synonyms <- c("dogs", "cat", "hound", "wolf")
  matched.synonyms <- which(synonyms$synonyms %in% known.synonyms)

  expect_true(length(matched.synonyms) > 0)
}

doTest("Test word2vec (Skip Gram, Hierarchical Softmax) on text8 dataset", test.word2vec.sg_hs)
