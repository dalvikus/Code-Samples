function Answer(timeout, start, end, result, selectedAnswer, confidenceLevel) {
    this.timeout = timeout  // in seconds
    this.start = start      // in milliseconds since Epoch (January 1, 1970 00:00:00.00)
    this.end = end          // ditto
    this.result = result    // boolean
    this.selectedAnswer = selectedAnswer
    this.confidenceLevel = confidenceLevel  // 1, 2, 3, 4, 5
}

export {Answer}
