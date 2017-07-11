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
////    console.log('BookmarkForm.handleClick: [' + index + ']: mid = "' + mid + '", title = "' + title + '"');
        this.props.onBookmarkButtonClick(mid, title);
    }

    handleDelete(e) {
        e.preventDefault();
        let index = e.target.value;
        let mid = this.props.bookmarks[index][0];
        let title = this.props.bookmarks[index][1];
////    console.log('BookmarkForm.handleDelete: [' + index + ']: mid = "' + mid + '", title = "' + title + '"');
        this.props.onDeleteBookmarkButtonClick(mid, title);
    }

    render() {
////  console.log('BookmarkForm.render()');
////  console.log('this.props.bookmarks: ' + this.props.bookmarks);
      let entries = [];
      for (let entry of this.props.bookmarks.entries()) {
        entries.push(entry);
      }
      return (
        <div>
          {entries.map((entry) =>
            <div key={entry[0]} style={{display: `inline`}}>
              <button value={entry[0]} onClick={this.handleClick}>{entry[1][1]}</button>
              <button value={entry[0]} onClick={this.handleDelete}>X</button>
            </div>
          )}
        </div>
      );
    }
}
