import React from 'react';

export class SearchForm extends React.Component {
    constructor(props) {
        super(props);

        let mid = props.mid;
        this.state = {
            'mid': mid,
            'title': '',
        };

        this.handleMidChange = this.handleMidChange.bind(this);
        this.handleTitleChange = this.handleTitleChange.bind(this);
        this.handleSubmit = this.handleSubmit.bind(this);
        this.handleBookmarkAdded = this.handleBookmarkAdded.bind(this);
    }

    handleMidChange(e) {
        this.setState({'mid': e.target.value});
////    console.log('SearchForm.handleMidChange: mid = "' + e.target.value + '"');
        document.getElementById("autoFocused").focus();
    }
    handleTitleChange(e) {
        this.setState({'title': e.target.value});
////    console.log('SearchForm.handleTitleChange: title = "' + e.target.value + '"');
    }
    handleSubmit(e) {
        e.preventDefault();
////    console.log('SearchForm.handleSubmit: mid = "' + this.state.mid + '", title = "' + this.state.title + '"');
        this.props.onSearchButtonClick(this.state.mid, this.state.title);
        document.getElementById("autoFocused").focus();
    }
    handleBookmarkAdded(e) {
        e.preventDefault();
        let mid = this.state.mid;
        let title = this.state.title;
////    console.log('SearchForm.handleBookmarkAdded: mid = "' + mid + '", title = "' + title + '"');
        if (title !== '') {
            this.props.onAddBookmarkButtonClick(mid, title);
        }
        document.getElementById("autoFocused").focus();
    }

    render() {
////  console.log('SearchForm.render(): mid = "' + this.state.mid + '", title = "' + this.state.title + '"');
      return (
        <form onSubmit={this.handleSubmit}>
          <select value={this.state.mid} onChange={this.handleMidChange}>
            {Object.keys(this.props.midsMap).map((mid) =>
              <option key={mid} value={mid}>{this.props.midsMap[mid]}</option>
            )}
          </select>
          <input id="autoFocused" type="text" value={this.state.title} onChange={this.handleTitleChange}/>
          <button type="submit">Search</button>
          <button onClick={this.handleBookmarkAdded}>bookmark</button>
        </form>
      );
    }
}
