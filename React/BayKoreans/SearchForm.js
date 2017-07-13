import React from 'react';
import './index.css';

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
        document.getElementById("query-text").focus();
    }
    handleTitleChange(e) {
        this.setState({'title': e.target.value});
    }
    handleSubmit(e) {
        e.preventDefault();
        this.props.onSearchButtonClick(this.state.mid, this.state.title);
        document.getElementById("query-text").focus();
    }
    handleBookmarkAdded(e) {
        e.preventDefault();
        let mid = this.state.mid;
        let title = this.state.title;
        if (title !== '') {
            this.props.onAddBookmarkButtonClick(mid, title);
        }
        document.getElementById("query-text").focus();
    }

    shouldComponentUpdate(nextProps, nextState) {
        return this.state.mid !== nextState.mid || this.state.title !== nextState.title;
    }

    render() {
      return (
        <form id="search-form" onSubmit={this.handleSubmit}>
          <select id="mid-select" value={this.state.mid} onChange={this.handleMidChange}>
            {Object.keys(this.props.midsMap).map((mid) =>
              <option key={mid} value={mid}>{this.props.midsMap[mid]}</option>
            )}
          </select>
          <input id="query-text" type="text" value={this.state.title} onChange={this.handleTitleChange}/>
          <button id="do-search-button" type="submit">Search</button>
          <button id="add-bookmark-button" onClick={this.handleBookmarkAdded}>bookmark</button>
        </form>
      );
    }
}
