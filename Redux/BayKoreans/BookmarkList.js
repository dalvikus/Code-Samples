import React from 'react'
import { connect } from 'react-redux'

const Bookmark = ({bookmark, onFetchClick, onDeleteClick}) => (
  <div style={{display: 'inline'}}>
    <button onClick={onFetchClick}>{bookmark.show.title}</button>
    <button onClick={onDeleteClick}>X</button>
  </div>
);

const BookmarkList = ({bookmarks, onFetchClick, onDeleteClick}) => (
  <div>
    {bookmarks.map(bookmark =>
      <Bookmark
        key={bookmark.id}
        bookmark={bookmark}
        onFetchClick = {() => {
            let index = bookmarks.map((e) => e.id).indexOf(bookmark.id);
            if (index === -1)
                console.error('no bookmark with id = ' + bookmark.id);
            else
                onFetchClick(bookmarks[index].show);
        }}
        onDeleteClick={() => onDeleteClick(bookmark.id)}
      />
    )}
  </div>
);

const mapStateToProps = (state) => ({
  bookmarks: state.bookmarks
});

const fetchShow = (show) => ({
  type: 'FETCH',
  show
});
const deleteBookmark = (id) => ({
  type: 'DELETE',
  id
});

const mapDispatchToProps = {
  onFetchClick: fetchShow,
  onDeleteClick: deleteBookmark
}

const VisibleBookmarkList = connect(
  mapStateToProps,
  mapDispatchToProps
)(BookmarkList);

export default VisibleBookmarkList;
