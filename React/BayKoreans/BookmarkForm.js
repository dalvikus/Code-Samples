import React from 'react';

export class BookmarkForm extends React.Component {
    constructor(props) {
        super(props);

        this.handleClick = this.handleClick.bind(this);
        this.handleDelete = this.handleDelete.bind(this);
    }

    handleClick(e) {
        e.preventDefault();
        let index = e.target.value;
        let mid = this.props.bookmarks[index][0];
        let title = this.props.bookmarks[index][1];
        this.props.onBookmarkButtonClick(mid, title);
    }

    handleDelete(e) {
        e.preventDefault();
        let index = e.target.value;
        let mid = this.props.bookmarks[index][0];
        let title = this.props.bookmarks[index][1];
        this.props.onDeleteBookmarkButtonClick(mid, title);
    }

    render() {
      return (
        <div id="bookmarks-div">
          {this.props.bookmarks.map((entry, index) =>
            <div className="bookmark-div" key={index}>
              <button className="bookmark-title-button" value={index} onClick={this.handleClick}>{entry[1]}</button>
              <button className="bookmark-delete-button" value={index} onClick={this.handleDelete}>X</button>
            </div>
          )}
        </div>
      );
    }
}
