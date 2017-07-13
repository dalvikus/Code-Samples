import React from 'react';

import {saveBookmarks, indexOf} from "./Bookmarks";
import {SearchForm} from "./SearchForm";
import {BookmarkForm} from "./BookmarkForm";
import {FetchEpisodes} from "./FetchEpisodes";

export class BayKoreans extends React.Component {
    constructor (props) {
        super(props);

        this.state = {
            'bookmarks': this.props.bookmarks,
            'mid': 'current',
            'title': ''
        };

        this.handleSearchButtonClick = this.handleSearchButtonClick.bind(this);
        this.handleAddBookmarkButtonClick = this.handleAddBookmarkButtonClick.bind(this);
        this.handleBookmarkButtonClick = this.handleBookmarkButtonClick.bind(this);
        this.handleDeleteBookmarkButtonClick = this.handleDeleteBookmarkButtonClick.bind(this);
    }

    handleSearchButtonClick(mid, title) {
        if (this.state.mid !== mid || this.state.title !== title)
            this.setState({'mid': mid, 'title': title});
    }

    handleAddBookmarkButtonClick(mid, title) {
        if (this.props.midsMap.hasOwnProperty(mid)) {
            if (title === '') {
                console.error('No title');
            } else {
                let bookmarks = this.state.bookmarks;
                let index = indexOf(bookmarks, mid, title);
                if (index === -1) {
                    bookmarks.push([mid, title]);
                    saveBookmarks(bookmarks);
                    this.setState({'bookmarks': bookmarks});
                }
            }
        } else {
            console.error('No mid: "' + mid + '"');
        }
    }

    handleBookmarkButtonClick(mid, title) {
        if (this.state.mid !== mid || this.state.title !== title)
            this.setState({'mid': mid, 'title': title});
        document.getElementById("query-text").focus();
    }

    handleDeleteBookmarkButtonClick(mid, title) {
        if (this.props.midsMap.hasOwnProperty(mid)) {
            if (title === '') {
                console.error('No title');
            } else {
                let bookmarks = this.state.bookmarks;
                let index = indexOf(bookmarks, mid, title);
                if (index === -1) {
                    console.error('No bookmark for mid: "' + mid + '", title: "' + title + '"');
                } else {
                    bookmarks.splice(index, 1);
                    saveBookmarks(bookmarks);
                    this.setState({'bookmarks': bookmarks});
                }
            }
        } else {
            console.error('No mid: "' + mid + '"');
        }
        document.getElementById("query-text").focus();
    }

    componentDidMount() {
        document.getElementById("query-text").focus();
    }

    render() {
        return (
          <div id="main-div">
            <SearchForm midsMap={this.props.midsMap} mid={this.state.mid} onSearchButtonClick={this.handleSearchButtonClick} onAddBookmarkButtonClick={this.handleAddBookmarkButtonClick} />
            <BookmarkForm bookmarks={this.state.bookmarks} onBookmarkButtonClick={this.handleBookmarkButtonClick} onDeleteBookmarkButtonClick={this.handleDeleteBookmarkButtonClick} />
            <FetchEpisodes midsMap={this.props.midsMap} mid={this.state.mid} title={this.state.title} />
          </div>
        );
    }
}
