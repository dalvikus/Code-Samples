import React from 'react';

import {saveBookmarks} from "./Bookmarks";
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
////    console.log('BayKoreans.handleSearchButtonClick: mid = "' + mid +'", title = "' + title + '"');
        this.setState({'mid': mid, 'title': title});
    }

    handleAddBookmarkButtonClick(mid, title) {
////    console.log('BayKoreans.handleAddBookmarkButtonClick: mid = "' + mid +'", title = "' + title + '"');
        if (this.props.midsMap.hasOwnProperty(mid)) {
            if (title === '') {
                console.error('No title');
            } else {
                let bookmarks = this.state.bookmarks;
                bookmarks.push([mid, title]);
                saveBookmarks(bookmarks);
                this.setState({'bookmarks': bookmarks});
            }
        } else {
            console.error('No mid: "' + mid + '"');
        }
    }

    handleBookmarkButtonClick(mid, title) {
////    console.log('BayKoreans.handleBookmarkButtonClick: mid = "' + mid +'", title = "' + title + '"');
        this.setState({'mid': mid, 'title': title});
        document.getElementById("autoFocused").focus();
    }

    handleDeleteBookmarkButtonClick(mid, title) {
////    console.log('BayKoreans.handleDeleteBookmarkButtonClick: mid = "' + mid +'", title = "' + title + '"');
        if (this.props.midsMap.hasOwnProperty(mid)) {
            if (title === '') {
                console.error('No title');
            } else {
                let bookmarks = this.state.bookmarks;
                let index = -1;
                for (let i = 0; i < bookmarks.length; ++i) {
                    let bookmark = bookmarks[i];
                    if (bookmark[0] === mid && bookmark[1] === title) {
                        index = i;
                        break;
                    }
                };
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
        document.getElementById("autoFocused").focus();
    }

    componentDidMount() {
        document.getElementById("autoFocused").focus();
    }
    render() {
////    console.log('BayKoreans.render: mid = "' + this.state.mid + '", title = "' + this.state.title + '"');
        return (
          <div>
            <SearchForm midsMap={this.props.midsMap} mid={this.state.mid} onSearchButtonClick={this.handleSearchButtonClick} onAddBookmarkButtonClick={this.handleAddBookmarkButtonClick} />
            <BookmarkForm bookmarks={this.state.bookmarks} onBookmarkButtonClick={this.handleBookmarkButtonClick} onDeleteBookmarkButtonClick={this.handleDeleteBookmarkButtonClick} />
            <FetchEpisodes midsMap={this.props.midsMap} mid={this.state.mid} title={this.state.title} />
          </div>
        );
    }
}
