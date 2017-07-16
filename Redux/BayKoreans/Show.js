function Show(mid, title) {
    this.mid = mid
    this.title = title
}

Show.prototype.toString = function showToString() {
    return encodeURIComponent('mid: "' + encodeURIComponent(this.mid) + '", title: "' + encodeURIComponent(this.title) + '"')
}

export {Show}
